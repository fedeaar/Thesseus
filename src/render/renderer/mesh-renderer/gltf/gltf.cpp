#define GLM_ENABLE_EXPERIMENTAL

#include "./gltf.h"
#include "fastgltf/types.hpp"
#include <glm/gtx/quaternion.hpp>
#include <variant>

//
// images
//

core::code
load_image_from_uri(mgmt::vulkan::Manager& ir_vkMgr,
                    fastgltf::sources::URI& ir_uri,
                    mgmt::vulkan::image::AllocatedImage& or_alloc)
{
  int width, height, nrChannels;
  if (ir_uri.fileByteOffset != 0) {
    core::Logger::err(FUNCTION_NAME, "image offsets not supported.");
    return core::code::NOT_IMPLEMENTED;
  }
  if (!ir_uri.uri.isLocalPath()) {
    core::Logger::err(FUNCTION_NAME, "non local paths not supported.");
    return core::code::NOT_IMPLEMENTED;
  }
  const std::string_view path_view = ir_uri.uri.path();
  const std::string path = { path_view.begin(), path_view.end() };
  unsigned char* data =
    core::io::image::raw(path, &width, &height, &nrChannels, 4);
  if (!data) {
    core::Logger::err(FUNCTION_NAME, "failed to read image.");
    return core::code::ERROR;
  }
  VkExtent3D extent;
  extent.width = width;
  extent.height = height;
  extent.depth = 1;
  auto status = ir_vkMgr.create_image(data,
                                      extent,
                                      VK_FORMAT_R8G8B8A8_UNORM,
                                      VK_IMAGE_USAGE_SAMPLED_BIT,
                                      false,
                                      or_alloc);
  core::io::image::free(data);
  return status;
}

core::code
load_image_from_vector(mgmt::vulkan::Manager& ir_vkMgr,
                       fastgltf::sources::Vector& ir_vector,
                       u32 offset,
                       u32 size,
                       mgmt::vulkan::image::AllocatedImage& or_alloc)
{
  int width, height, nrChannels;
  unsigned char* data =
    core::io::image::raw((u8*)ir_vector.bytes.data() + offset,
                         static_cast<int>(size),
                         &width,
                         &height,
                         &nrChannels,
                         4);
  if (!data) {
    core::Logger::err(FUNCTION_NAME, "failed to read vector data.");
    return core::code::ERROR;
  }
  VkExtent3D extent;
  extent.width = width;
  extent.height = height;
  extent.depth = 1;
  auto status = ir_vkMgr.create_image(data,
                                      extent,
                                      VK_FORMAT_R8G8B8A8_UNORM,
                                      VK_IMAGE_USAGE_SAMPLED_BIT,
                                      false,
                                      or_alloc);
  core::io::image::free(data);
  return status;
}

core::code
load_image_from_array(mgmt::vulkan::Manager& ir_vkMgr,
                      fastgltf::sources::Array& ir_array,
                      u32 offset,
                      u32 size,
                      mgmt::vulkan::image::AllocatedImage& or_alloc)
{
  int width, height, nrChannels;
  unsigned char* data =
    core::io::image::raw((u8*)ir_array.bytes.data() + offset,
                         static_cast<int>(size),
                         &width,
                         &height,
                         &nrChannels,
                         4);
  if (!data) {
    core::Logger::err(FUNCTION_NAME, "failed to read array data.");
    return core::code::ERROR;
  }
  VkExtent3D extent;
  extent.width = width;
  extent.height = height;
  extent.depth = 1;
  auto status = ir_vkMgr.create_image(data,
                                      extent,
                                      VK_FORMAT_R8G8B8A8_UNORM,
                                      VK_IMAGE_USAGE_SAMPLED_BIT,
                                      false,
                                      or_alloc);
  core::io::image::free(data);
  return status;
}

core::code
load_image(mgmt::vulkan::Manager& ir_vkMgr,
           fastgltf::Asset& ir_asset,
           fastgltf::Image& ir_image,
           mgmt::vulkan::image::AllocatedImage& or_alloc)
{
  auto status = core::code::ERROR;
  std::visit(
    fastgltf::visitor{
      [&](fastgltf::sources::URI& ir_uri) {
        status = load_image_from_uri(ir_vkMgr, ir_uri, or_alloc);
      },
      [&](fastgltf::sources::Vector& ir_vector) {
        status = load_image_from_vector(
          ir_vkMgr, ir_vector, 0, ir_vector.bytes.size(), or_alloc);
      },
      [&](fastgltf::sources::BufferView& ir_view) {
        auto& bufferView = ir_asset.bufferViews[ir_view.bufferViewIndex];
        auto& buffer = ir_asset.buffers[bufferView.bufferIndex];
        std::visit(fastgltf::visitor{
                     [&](fastgltf::sources::Array& ir_array) {
                       status = load_image_from_array(ir_vkMgr,
                                                      ir_array,
                                                      bufferView.byteOffset,
                                                      bufferView.byteLength,
                                                      or_alloc);
                     },
                     [&](fastgltf::sources::Vector& ir_vector) {
                       status = load_image_from_vector(ir_vkMgr,
                                                       ir_vector,
                                                       bufferView.byteOffset,
                                                       bufferView.byteLength,
                                                       or_alloc);
                     },
                     [&](auto& ir_other) {
                       core::Logger::wrn(
                         FUNCTION_NAME,
                         "tried to load unimplemented buffer format.");
                       status = core::code::NOT_IMPLEMENTED;
                     },
                   },
                   buffer.data);
      },
      [&](auto& ir_other) {
        core::Logger::wrn(FUNCTION_NAME,
                          "tried to load unimplemented image format.");
        status = core::code::NOT_IMPLEMENTED;
      },
    },
    ir_image.data);
  return status;
}

core::code
load_images(mgmt::vulkan::Manager& ir_vkMgr,
            fastgltf::Asset& ir_asset,
            mgmt::vulkan::image::AllocatedImage& ir_default_image,
            std::vector<mgmt::vulkan::image::AllocatedImage>& mr_images,
            render::asset::LoadedGLTF& mr_scene)
{
  for (fastgltf::Image& image : ir_asset.images) {
    mgmt::vulkan::image::AllocatedImage alloc;
    auto status = load_image(ir_vkMgr, ir_asset, image, alloc);
    if (status == core::code::SUCCESS) {
      mr_images.push_back(alloc);
      mr_scene.images[image.name.c_str()] = alloc;
    } else {
      core::Logger::wrn(FUNCTION_NAME, "failed to load image {}", image.name);
      mr_images.push_back(ir_default_image);
    }
  }
  return core::code::SUCCESS;
}

//
// samplers
//

core::code
load_samplers(mgmt::vulkan::Manager& ir_vkMgr,
              fastgltf::Asset& ir_asset,
              render::asset::LoadedGLTF& mr_scene)
{
  auto& dev = ir_vkMgr.get_dev();
  for (fastgltf::Sampler& sampler : ir_asset.samplers) {
    VkSamplerCreateInfo sampleInfo = { .sType =
                                         VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                                       .pNext = nullptr };
    sampleInfo.maxLod = VK_LOD_CLAMP_NONE;
    sampleInfo.minLod = 0;
    sampleInfo.magFilter = core::io::gltf::to_vulkan_filter(
      sampler.magFilter.value_or(fastgltf::Filter::Nearest));
    sampleInfo.minFilter = core::io::gltf::to_vulkan_filter(
      sampler.minFilter.value_or(fastgltf::Filter::Nearest));
    sampleInfo.mipmapMode = core::io::gltf::to_vulkan_mipmap_mode(
      sampler.minFilter.value_or(fastgltf::Filter::Nearest));
    VkSampler newSampler;
    vkCreateSampler(
      dev, &sampleInfo, nullptr, &newSampler); // TODO: create vulkan function?
    mr_scene.samplers.push_back(newSampler);
  }
  return core::code::SUCCESS;
}

//
// pools
//

core::code
init_pools(mgmt::vulkan::Manager& ir_vkMgr,
           fastgltf::Asset& ir_asset,
           render::asset::LoadedGLTF& mr_scene)
{
  std::vector<mgmt::vulkan::descriptor::PoolSizeRatio> sizes = {
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 }
  };
  mr_scene.descriptor_pool.init(
    ir_vkMgr.get_dev(), ir_asset.materials.size(), sizes);
  return core::code::SUCCESS;
}

//
// materials
//

core::code
load_materials_buffer(
  mgmt::vulkan::Manager& ir_vkMgr,
  mgmt::vulkan::descriptor::Writer& ir_writer,
  render::asset::material::MaterialPipelines& ir_materialPipes,
  render::asset::DefaultResources& ir_defaultRes,
  fastgltf::Asset& ir_asset,
  std::vector<mgmt::vulkan::image::AllocatedImage>& ir_images,
  std::vector<std::shared_ptr<render::asset::material::Material>>& mr_materials,
  render::asset::LoadedGLTF& mr_scene)
{
  auto res = ir_vkMgr.create_buffer(
    sizeof(render::asset::material::GPUMaterialConstants) *
      ir_asset.materials.size(),
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    VMA_MEMORY_USAGE_CPU_TO_GPU);
  if (!res.has_value()) {
    core::Logger::err(FUNCTION_NAME, "failed to create materials buffer.");
    return core::code::ERROR;
  }
  mr_scene.material_data_buff = res.value();
  u32 data_index = 0;
  render::asset::material::GPUMaterialConstants* scene_material_constants;
  vmaMapMemory(ir_vkMgr.get_allocator(),
               mr_scene.material_data_buff.allocation,
               (void**)&scene_material_constants);
  for (fastgltf::Material& mat : ir_asset.materials) {
    std::shared_ptr<render::asset::material::Material> new_material =
      std::make_shared<render::asset::material::Material>();
    mr_materials.push_back(new_material);
    mr_scene.materials[mat.name.c_str()] = new_material;
    render::asset::material::GPUMaterialConstants constants;
    constants.color_factors.x =
      core::clamp(mat.pbrData.baseColorFactor[0], 1e-5f, 1e5f);
    constants.color_factors.y =
      core::clamp(mat.pbrData.baseColorFactor[1], 1e-5f, 1e5f);
    constants.color_factors.z =
      core::clamp(mat.pbrData.baseColorFactor[2], 1e-5f, 1e5f);
    constants.color_factors.w =
      core::clamp(mat.pbrData.baseColorFactor[3], 1e-5f, 1e5f);
    constants.metal_rough_factors.x =
      core::clamp(mat.pbrData.metallicFactor, 1e-5f, 1e5f);
    constants.metal_rough_factors.y =
      core::clamp(mat.pbrData.roughnessFactor, 1e-5f, 1e5f);
    scene_material_constants[data_index] = constants;
    render::asset::material::Type passType =
      render::asset::material::Type::opaque;
    if (mat.alphaMode == fastgltf::AlphaMode::Blend) {
      passType = render::asset::material::Type::transparent;
    }
    render::asset::material::MaterialResources resources;
    resources.color_img = ir_defaultRes.white_img;
    resources.color_sampler = ir_defaultRes.default_linear_sampler;
    resources.metal_img = ir_defaultRes.white_img;
    resources.metal_sampler = ir_defaultRes.default_linear_sampler;
    resources.data_buff = mr_scene.material_data_buff.buffer;
    resources.data_buff_offset =
      data_index * sizeof(render::asset::material::GPUMaterialConstants);
    if (mat.pbrData.baseColorTexture.has_value()) {
      u32 img =
        ir_asset.textures[mat.pbrData.baseColorTexture.value().textureIndex]
          .imageIndex.value();
      u32 sampler =
        ir_asset.textures[mat.pbrData.baseColorTexture.value().textureIndex]
          .samplerIndex.value();
      resources.color_img = ir_images[img];
      resources.color_sampler = mr_scene.samplers[sampler];
    }
    // build material
    new_material->data =
      render::asset::material::write_material(passType,
                                              resources,
                                              ir_vkMgr,
                                              mr_scene.descriptor_pool,
                                              ir_writer,
                                              ir_materialPipes);
    data_index++;
  }
  vmaUnmapMemory(ir_vkMgr.get_allocator(),
                 mr_scene.material_data_buff.allocation);
}

//
// load_meshes
//

core::Result<render::asset::mesh::GPUMeshBuffers, core::code>
upload_mesh(mgmt::vulkan::Manager& ir_vkMgr,
            std::vector<u32>& ir_idxs,
            std::vector<render::asset::mesh::Vertex>& ir_vtxs)
{
  VkDevice dev = ir_vkMgr.get_dev();
  const size_t vb_s = ir_vtxs.size() * sizeof(render::asset::mesh::Vertex);
  const size_t ib_s = ir_idxs.size() * sizeof(u32);
  render::asset::mesh::GPUMeshBuffers mesh;
  auto vertex_buff_result = ir_vkMgr.create_buffer(
    vb_s,
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);
  if (!vertex_buff_result.has_value()) {
    core::Logger::err(FUNCTION_NAME, "could not create vertex buffer");
    return core::code::ERROR;
  }
  mesh.vertex_buff = vertex_buff_result.value();
  VkBufferDeviceAddressInfo addr_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .buffer = mesh.vertex_buff.buffer
  };
  mesh.vertex_buff_addr = vkGetBufferDeviceAddress(dev, &addr_info);
  auto index_buff_result = ir_vkMgr.create_buffer(
    ib_s,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);
  if (!index_buff_result.has_value()) {
    core::Logger::err(FUNCTION_NAME, "could not create index buffer");
    return core::code::ERROR;
  }
  mesh.index_buff = index_buff_result.value();
  auto staging_result = ir_vkMgr.create_buffer(
    vb_s + ib_s, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
  if (!index_buff_result.has_value()) {
    core::Logger::err(FUNCTION_NAME, "could not create staging buffer");
    return core::code::ERROR;
  }
  auto staging = staging_result.value();
  auto alloc = ir_vkMgr.get_allocator();
  void* data;
  vmaMapMemory(alloc, staging.allocation, &data);
  memcpy(data, ir_vtxs.data(), vb_s);
  memcpy((char*)data + vb_s, ir_idxs.data(), ib_s);
  vmaUnmapMemory(alloc, staging.allocation);
  ir_vkMgr.imm_submit([&](VkCommandBuffer cmd) {
    VkBufferCopy vertex_copy{ 0 };
    vertex_copy.dstOffset = 0;
    vertex_copy.srcOffset = 0;
    vertex_copy.size = vb_s;
    vkCmdCopyBuffer(
      cmd, staging.buffer, mesh.vertex_buff.buffer, 1, &vertex_copy);
    VkBufferCopy index_copy{ 0 };
    index_copy.dstOffset = 0;
    index_copy.srcOffset = vb_s;
    index_copy.size = ib_s;
    vkCmdCopyBuffer(
      cmd, staging.buffer, mesh.index_buff.buffer, 1, &index_copy);
  });
  ir_vkMgr.destroy_buffer(staging);
  return mesh;
};

core::code
load_mesh_indices(fastgltf::Asset& ir_asset,
                  fastgltf::Primitive& ir_primitive,
                  u32 i_initialVtx,
                  std::vector<u32>& mr_idxs,
                  render::asset::mesh::Surface& mr_newSurface)
{
  if (!ir_primitive.indicesAccessor.has_value()) {
    core::Logger::err(FUNCTION_NAME,
                      "gltf asset has an invalid indices accessor.");
    return core::code::ERROR;
  }
  auto& idx_acc = ir_asset.accessors[ir_primitive.indicesAccessor.value()];
  mr_newSurface.count = (u32)idx_acc.count;
  mr_idxs.reserve(mr_idxs.size() + idx_acc.count);
  fastgltf::iterateAccessor<u32>(
    ir_asset, idx_acc, [&](u32 idx) { mr_idxs.push_back(idx + i_initialVtx); });
};

core::code
load_mesh_positions(fastgltf::Asset& ir_asset,
                    fastgltf::Primitive& ir_primitive,
                    u32 i_initialVtx,
                    std::vector<render::asset::mesh::Vertex>& mr_vtxs)
{
  auto pos = ir_primitive.findAttribute("POSITION");
  if (pos == ir_primitive.attributes.end()) {
    core::Logger::err(FUNCTION_NAME,
                      "gltf asset has an invalid position attribute.");
    return core::code::ERROR;
  }
  auto& pos_acc = ir_asset.accessors[pos->accessorIndex];
  if (!pos_acc.bufferViewIndex.has_value()) {
    return core::code::SUCCESS;
  }
  mr_vtxs.resize(mr_vtxs.size() + pos_acc.count);
  fastgltf::iterateAccessorWithIndex<v3f>(
    ir_asset, pos_acc, [&](v3f raw, size_t idx) {
      render::asset::mesh::Vertex vtx;
      vtx.position = raw;
      vtx.normal = { 1, 0, 0 };
      vtx.color = v4f{ 1.f };
      vtx.uv_x = 0;
      vtx.uv_y = 0;
      mr_vtxs[i_initialVtx + idx] = vtx;
    });
}

core::code
load_mesh_normals(fastgltf::Asset& ir_asset,
                  fastgltf::Primitive& ir_primitive,
                  u32 i_initialVtx,
                  std::vector<render::asset::mesh::Vertex>& mr_vtxs)
{
  auto normals = ir_primitive.findAttribute("NORMAL");
  if (normals != ir_primitive.attributes.end()) {
    auto& normals_acc = ir_asset.accessors[normals->accessorIndex];
    fastgltf::iterateAccessorWithIndex<v3f>(
      ir_asset, normals_acc, [&](v3f v, size_t index) {
        mr_vtxs[i_initialVtx + index].normal = v;
      });
  }
}

core::code
load_mesh_uvs(fastgltf::Asset& ir_asset,
              fastgltf::Primitive& ir_primitive,
              u32 i_initialVtx,
              std::vector<render::asset::mesh::Vertex>& mr_vtxs)
{
  auto uv = ir_primitive.findAttribute("TEXCOORD_0");
  if (uv != ir_primitive.attributes.end()) {
    auto& uv_acc = ir_asset.accessors[uv->accessorIndex];
    fastgltf::iterateAccessorWithIndex<v2f>(
      ir_asset, uv_acc, [&](v2f v, size_t index) {
        mr_vtxs[i_initialVtx + index].uv_x = v.x;
        mr_vtxs[i_initialVtx + index].uv_y = v.y;
      });
  }
}

core::code
load_mesh_colors(fastgltf::Asset& ir_asset,
                 fastgltf::Primitive& ir_primitive,
                 u32 i_initialVtx,
                 std::vector<render::asset::mesh::Vertex>& mr_vtxs)
{
  auto colors = ir_primitive.findAttribute("COLOR_0");
  if (colors != ir_primitive.attributes.end()) {
    auto& colors_acc = ir_asset.accessors[colors->accessorIndex];
    fastgltf::iterateAccessorWithIndex<v4f>(
      ir_asset, colors_acc, [&](v4f v, size_t index) {
        mr_vtxs[i_initialVtx + index].color = v;
      });
  }
}

core::code
load_mesh_materials(
  fastgltf::Asset& ir_asset,
  fastgltf::Primitive& ir_primitive,
  std::vector<std::shared_ptr<render::asset::material::Material>>& ir_materials,
  u32 i_initialVtx,
  std::vector<render::asset::mesh::Vertex>& mr_vtxs,
  render::asset::mesh::Surface& mr_newSurface)
{
  if (ir_primitive.materialIndex.has_value()) {
    mr_newSurface.material = ir_materials[ir_primitive.materialIndex.value()];
  } else {
    mr_newSurface.material = ir_materials[0];
  }
}

core::code
load_meshes(
  mgmt::vulkan::Manager& ir_vkMgr,
  fastgltf::Asset& ir_asset,
  std::vector<std::shared_ptr<render::asset::material::Material>>& ir_materials,
  std::vector<std::shared_ptr<render::asset::mesh::Mesh>>& mr_meshes,
  render::asset::LoadedGLTF& mr_scene)
{
  std::vector<u32> idxs;
  std::vector<render::asset::mesh::Vertex> vtxs;
  for (auto& ir_mesh : ir_asset.meshes) {
    std::shared_ptr<render::asset::mesh::Mesh> p_newMesh =
      std::make_shared<render::asset::mesh::Mesh>();
    auto& new_mesh = *(p_newMesh.get());
    for (auto&& it : ir_mesh.primitives) {
      render::asset::mesh::Surface new_surface;
      new_surface.start_idx = (u32)idxs.size();
      u32 initial_vtx = vtxs.size();
      // load indices
      load_mesh_indices(ir_asset, it, initial_vtx, idxs, new_surface);
      // load vertex positions
      load_mesh_positions(ir_asset, it, initial_vtx, vtxs);
      // load vertex normals
      load_mesh_normals(ir_asset, it, initial_vtx, vtxs);
      // load UVs
      load_mesh_uvs(ir_asset, it, initial_vtx, vtxs);
      // load vertex colors
      load_mesh_colors(ir_asset, it, initial_vtx, vtxs);
      // load material
      load_mesh_materials(
        ir_asset, it, ir_materials, initial_vtx, vtxs, new_surface);
      new_mesh.surfaces.push_back(new_surface);
    }
    auto upload_result = upload_mesh(ir_vkMgr, idxs, vtxs);
    if (!upload_result.has_value()) {
      core::Logger::err(FUNCTION_NAME, "could not load gltf asset to gpu.");
      return core::code::ERROR;
    }
    new_mesh.mesh_buffers = upload_result.value();
    new_mesh.name = ir_mesh.name;
    mr_scene.meshes[new_mesh.name.c_str()] = p_newMesh;
    mr_meshes.push_back(p_newMesh);
    vtxs.clear();
    idxs.clear();
  }
};

core::code
load_nodes(fastgltf::Asset& ir_asset,
           std::vector<std::shared_ptr<render::asset::mesh::Mesh>>& ir_meshes,
           std::vector<std::shared_ptr<render::asset::Node>>& mr_nodes,
           render::asset::LoadedGLTF& mr_scene)
{
  for (fastgltf::Node& node : ir_asset.nodes) {
    std::shared_ptr<render::asset::Node> newNode;
    if (node.meshIndex.has_value()) {
      newNode = std::make_shared<render::asset::MeshNode>();
      static_cast<render::asset::MeshNode*>(newNode.get())->mesh =
        ir_meshes[*node.meshIndex];
    } else {
      newNode = std::make_shared<render::asset::Node>();
    }
    std::visit(
      fastgltf::visitor{
        [&](fastgltf::math::fmat4x4 matrix) {
          memcpy(&newNode->local_tf, matrix.data(), sizeof(matrix));
        },
        [&](fastgltf::TRS transform) {
          v3f tv(transform.translation[0],
                 transform.translation[1],
                 transform.translation[2]);
          glm::quat rq(transform.rotation[3],
                       transform.rotation[0],
                       transform.rotation[1],
                       transform.rotation[2]);
          v3f sv(transform.scale[0], transform.scale[1], transform.scale[2]);
          m4f tm = glm::translate(m4f(1.f), tv);
          m4f rm = glm::toMat4(rq);
          m4f sm = glm::scale(m4f(1.f), sv);
          newNode->local_tf = tm * rm * sm;
        },
        [&](auto& ir_other) {
          core::Logger::wrn(FUNCTION_NAME,
                            "tried to load unimplemented transform format.");
        } },
      node.transform);
    mr_nodes.push_back(newNode);
    mr_scene.nodes[node.name.c_str()] = newNode;
  }
  // set graph
  for (int i = 0; i < ir_asset.nodes.size(); i++) {
    fastgltf::Node& node = ir_asset.nodes[i];
    std::shared_ptr<render::asset::Node>& sceneNode = mr_nodes[i];
    for (auto& c : node.children) {
      sceneNode->children.push_back(mr_nodes[c]);
      mr_nodes[c]->parent = sceneNode;
    }
  }
  // set top nodes
  for (auto& node : mr_nodes) {
    if (node->parent.lock() == nullptr) {
      mr_scene.top_nodes.push_back(node);
      node->refresh_transform(glm::mat4{ 1.f });
    }
  }
}

core::code
render::gltf::load_gltf(
  char* ip_path,
  mgmt::vulkan::Manager& ir_vkMgr,
  render::asset::DefaultResources& ir_defaultRes,
  mgmt::vulkan::descriptor::Writer& ir_writer,
  render::asset::material::MaterialPipelines& ir_materialPipes,
  std::shared_ptr<render::asset::LoadedGLTF>& mr_sene)
{
  auto status = core::code::ERROR;
  render::asset::LoadedGLTF& scene = *mr_sene.get();
  // load file
  fastgltf::Asset asset;
  status = core::io::gltf::load(ip_path, &asset);
  if (status != core::code::SUCCESS) {
    core::Logger::err(
      FUNCTION_NAME, "failed to load gltf asset. code: {}.", (u32)status);
    return core::code::ERROR;
  }
  // init pools
  init_pools(ir_vkMgr, asset, scene);
  // init sampler
  load_samplers(ir_vkMgr, asset, scene);
  // load all textures
  std::vector<mgmt::vulkan::image::AllocatedImage> images;
  load_images(ir_vkMgr, asset, ir_defaultRes.error_checker_img, images, scene);
  // load materials
  std::vector<std::shared_ptr<render::asset::material::Material>> materials;
  load_materials_buffer(ir_vkMgr,
                        ir_writer,
                        ir_materialPipes,
                        ir_defaultRes,
                        asset,
                        images,
                        materials,
                        scene);
  // load meshes
  std::vector<std::shared_ptr<render::asset::mesh::Mesh>> meshes;
  load_meshes(ir_vkMgr, asset, materials, meshes, scene);
  // load nodes
  std::vector<std::shared_ptr<render::asset::Node>> nodes;
  load_nodes(asset, meshes, nodes, scene);
  scene.vk_mgr = &ir_vkMgr;
  scene.initialized = true;
}
