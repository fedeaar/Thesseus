#define GLM_ENABLE_EXPERIMENTAL

#include "./gltf.h"
#include "fastgltf/types.hpp"
#include <glm/gtx/quaternion.hpp>
#include <variant>

//
// constructor
//

core::code
load_image_from_uri(mgmt::vulkan::Manager& ir_vkMgr,
                    fastgltf::sources::URI& ir_uri,
                    mgmt::vulkan::image::AllocatedImage& or_alloc)
{
  int width, height, nrChannels;
  if (ir_uri.fileByteOffset != 0) {
    ERR("image offsets not supported");
    return core::code::NOT_IMPLEMENTED;
  }
  if (!ir_uri.uri.isLocalPath()) {
    ERR("non local paths not supported");
    return core::code::NOT_IMPLEMENTED;
  }
  const std::string_view pathView = ir_uri.uri.path();
  const std::string path = { pathView.begin(), pathView.end() };
  unsigned char* data =
    core::io::image::raw(path, &width, &height, &nrChannels, 4);
  if (!data) {
    ERR("failed to read image");
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
    ERR("failed to read vector data.");
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
    ERR("failed to read array data.");
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
                       WRN("tried to load unimplemented buffer format.");
                       status = core::code::NOT_IMPLEMENTED;
                     },
                   },
                   buffer.data);
      },
      [&](auto& ir_other) {
        WRN("tried to load unimplemented image format.");
        status = core::code::NOT_IMPLEMENTED;
      },
    },
    ir_image.data);
  return status;
}

core::code
render::gltf::GLTFScene::init_images(
  fastgltf::Asset& ir_asset,
  mgmt::vulkan::image::AllocatedImage& ir_default_image,
  std::vector<mgmt::vulkan::image::AllocatedImage>& mr_images)
{
  for (fastgltf::Image& image : ir_asset.images) {
    mgmt::vulkan::image::AllocatedImage alloc;
    auto status = load_image(*p_vkMgr_, ir_asset, image, alloc);
    if (status == core::code::SUCCESS) {
      mr_images.push_back(alloc);
      images[image.name.c_str()] = alloc;
    } else {
      WRN("failed to load image {}", image.name);
      mr_images.push_back(ir_default_image);
    }
  }
  return core::code::SUCCESS;
}

core::code
render::gltf::GLTFScene::init_samplers(fastgltf::Asset& ir_asset)
{
  auto& dev = p_vkMgr_->get_dev();
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
      dev, &sampleInfo, nullptr, &newSampler); // TODO: create mgmt function?
    samplers.push_back(newSampler);
  }
  return core::code::SUCCESS;
}

core::code
render::gltf::GLTFScene::init_pools(fastgltf::Asset& ir_asset)
{
  std::vector<mgmt::vulkan::descriptor::PoolSizeRatio> sizes = {
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 }
  };
  descriptorPool.init(p_vkMgr_->get_dev(), ir_asset.materials.size(), sizes);
  return core::code::SUCCESS;
}

core::code
render::gltf::GLTFScene::init_materials(
  mgmt::vulkan::descriptor::Writer& ir_writer,
  render::MaterialPassPipelines& ir_materialPipes,
  render::DefaultResources& ir_defaultRes,
  fastgltf::Asset& ir_asset,
  std::vector<mgmt::vulkan::image::AllocatedImage>& ir_images,
  std::vector<std::shared_ptr<render::Material>>& mr_materials)
{
  auto& vkMgr = *p_vkMgr_;
  auto createBufferResult = vkMgr.create_buffer(
    sizeof(render::GPUMaterialConstants) * ir_asset.materials.size(),
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    VMA_MEMORY_USAGE_CPU_TO_GPU);
  if (!createBufferResult.has_value()) {
    ERR("failed to create materials buffer.");
    return core::code::ERROR;
  }
  materialDataBuff = createBufferResult.value();
  u32 idx = 0;
  render::GPUMaterialConstants* sceneMaterialConsts;
  vmaMapMemory(vkMgr.get_allocator(),
               materialDataBuff.allocation,
               (void**)&sceneMaterialConsts); // TODO: handle errors
  for (fastgltf::Material& material : ir_asset.materials) {
    std::shared_ptr<render::Material> newMaterial =
      std::make_shared<render::Material>();
    mr_materials.push_back(newMaterial);
    materials[material.name.c_str()] = newMaterial;
    render::GPUMaterialConstants constants {
      .colorFactors = {      
        core::clamp(material.pbrData.baseColorFactor[0], 1e-5f, 1e5f),
        core::clamp(material.pbrData.baseColorFactor[1], 1e-5f, 1e5f),
        core::clamp(material.pbrData.baseColorFactor[2], 1e-5f, 1e5f),
        core::clamp(material.pbrData.baseColorFactor[3], 1e-5f, 1e5f),
      },
      .metalRoughFactors = {
        core::clamp(material.pbrData.metallicFactor, 1e-5f, 1e5f),
        core::clamp(material.pbrData.roughnessFactor, 1e-5f, 1e5f),
        0,
        0
      }, 
    };
    sceneMaterialConsts[idx] = constants;
    auto passType = render::MaterialPassType::opaque;
    if (material.alphaMode == fastgltf::AlphaMode::Blend) {
      passType = render::MaterialPassType::transparent;
    }
    render::MaterialResources resources = {
      .colorImg = ir_defaultRes.whiteImg,
      .metalImg = ir_defaultRes.whiteImg,
      .p_colorSampler = ir_defaultRes.p_linearSampler,
      .p_metalSampler = ir_defaultRes.p_linearSampler,
      .p_dataBuff = materialDataBuff.buffer,
      .dataBuffOffset =
        static_cast<u32>(idx * sizeof(render::GPUMaterialConstants))
    };
    if (material.pbrData.baseColorTexture.has_value()) {
      u32 img =
        ir_asset
          .textures[material.pbrData.baseColorTexture.value().textureIndex]
          .imageIndex.value();
      u32 sampler =
        ir_asset
          .textures[material.pbrData.baseColorTexture.value().textureIndex]
          .samplerIndex.value();
      resources.colorImg = ir_images[img];
      resources.p_colorSampler = samplers[sampler];
    }
    // build material
    newMaterial->data = render::write_material(
      passType, resources, vkMgr, descriptorPool, ir_writer, ir_materialPipes);
    idx++;
  }
  vmaUnmapMemory(vkMgr.get_allocator(), materialDataBuff.allocation);
  return core::code::SUCCESS;
}

core::code
upload_mesh(mgmt::vulkan::Manager& ir_vkMgr,
            std::vector<u32>& ir_idxs,
            std::vector<render::MeshVertex>& ir_vtxs,
            render::GPUMeshBuffers& or_meshBuffers)
{
  VkDevice dev = ir_vkMgr.get_dev();
  const u32 vbSize = ir_vtxs.size() * sizeof(render::MeshVertex);
  const u32 ibSize = ir_idxs.size() * sizeof(u32);
  render::GPUMeshBuffers meshBuffers;
  auto vertexBuffResult = ir_vkMgr.create_buffer(
    vbSize,
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);
  if (!vertexBuffResult.has_value()) {
    ERR("could not create vertex buffer");
    return core::code::ERROR;
  }
  meshBuffers.vertexBuff = vertexBuffResult.value();
  VkBufferDeviceAddressInfo addrInfo = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .buffer = meshBuffers.vertexBuff.buffer
  };
  meshBuffers.vba = vkGetBufferDeviceAddress(dev, &addrInfo);
  auto idxBuffResult = ir_vkMgr.create_buffer(
    ibSize,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);
  if (!idxBuffResult.has_value()) {
    ERR("could not create index buffer");
    return core::code::ERROR;
  }
  meshBuffers.idxBuff = idxBuffResult.value();
  auto stagingResult = ir_vkMgr.create_buffer(vbSize + ibSize,
                                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                              VMA_MEMORY_USAGE_CPU_ONLY);
  if (!idxBuffResult.has_value()) {
    ERR("could not create staging buffer");
    return core::code::ERROR;
  }
  auto staging = stagingResult.value();
  auto alloc = ir_vkMgr.get_allocator();
  void* data;
  vmaMapMemory(alloc, staging.allocation, &data);
  memcpy(data, ir_vtxs.data(), vbSize);
  memcpy((char*)data + vbSize, ir_idxs.data(), ibSize);
  vmaUnmapMemory(alloc, staging.allocation);
  ir_vkMgr.imm_submit([&](VkCommandBuffer cmd) {
    VkBufferCopy vertexCopy{ 0 };
    vertexCopy.dstOffset = 0;
    vertexCopy.srcOffset = 0;
    vertexCopy.size = vbSize;
    vkCmdCopyBuffer(
      cmd, staging.buffer, meshBuffers.vertexBuff.buffer, 1, &vertexCopy);
    VkBufferCopy idxCopy{ 0 };
    idxCopy.dstOffset = 0;
    idxCopy.srcOffset = vbSize;
    idxCopy.size = ibSize;
    vkCmdCopyBuffer(
      cmd, staging.buffer, meshBuffers.idxBuff.buffer, 1, &idxCopy);
  });
  ir_vkMgr.destroy_buffer(staging);
  or_meshBuffers = meshBuffers;
  return core::code::SUCCESS;
};

core::code
load_mesh_indices(fastgltf::Asset& ir_asset,
                  fastgltf::Primitive& ir_primitive,
                  u32 i_initialVtx,
                  std::vector<u32>& mr_idxs,
                  render::MeshSurface& mr_newSurface)
{
  if (!ir_primitive.indicesAccessor.has_value()) {
    core::Logger::err(FUNCTION_NAME,
                      "gltf asset has an invalid indices accessor.");
    return core::code::ERROR;
  }
  auto& idxAcc = ir_asset.accessors[ir_primitive.indicesAccessor.value()];
  mr_newSurface.count = (u32)idxAcc.count;
  mr_idxs.reserve(mr_idxs.size() + idxAcc.count);
  fastgltf::iterateAccessor<u32>(
    ir_asset, idxAcc, [&](u32 idx) { mr_idxs.push_back(idx + i_initialVtx); });
};

core::code
load_mesh_positions(fastgltf::Asset& ir_asset,
                    fastgltf::Primitive& ir_primitive,
                    u32 i_initialVtx,
                    std::vector<render::MeshVertex>& mr_vtxs)
{
  auto pos = ir_primitive.findAttribute("POSITION");
  if (pos == ir_primitive.attributes.end()) {
    core::Logger::err(FUNCTION_NAME,
                      "gltf asset has an invalid position attribute.");
    return core::code::ERROR;
  }
  auto& posAcc = ir_asset.accessors[pos->accessorIndex];
  if (!posAcc.bufferViewIndex.has_value()) {
    return core::code::SUCCESS;
  }
  mr_vtxs.resize(mr_vtxs.size() + posAcc.count);
  fastgltf::iterateAccessorWithIndex<v3f>(
    ir_asset, posAcc, [&](v3f raw, size_t idx) {
      render::MeshVertex vtx;
      vtx.position = raw;
      vtx.normal = { 1, 0, 0 };
      vtx.color = v4f{ 1.f };
      vtx.uvX = 0;
      vtx.uvY = 0;
      mr_vtxs[i_initialVtx + idx] = vtx;
    });
}

core::code
load_mesh_normals(fastgltf::Asset& ir_asset,
                  fastgltf::Primitive& ir_primitive,
                  u32 i_initialVtx,
                  std::vector<render::MeshVertex>& mr_vtxs)
{
  auto normals = ir_primitive.findAttribute("NORMAL");
  if (normals != ir_primitive.attributes.end()) {
    auto& normalsAcc = ir_asset.accessors[normals->accessorIndex];
    fastgltf::iterateAccessorWithIndex<v3f>(
      ir_asset, normalsAcc, [&](v3f v, size_t index) {
        mr_vtxs[i_initialVtx + index].normal = v;
      });
  }
}

core::code
load_mesh_uvs(fastgltf::Asset& ir_asset,
              fastgltf::Primitive& ir_primitive,
              u32 i_initialVtx,
              std::vector<render::MeshVertex>& mr_vtxs)
{
  auto uv = ir_primitive.findAttribute("TEXCOORD_0");
  if (uv != ir_primitive.attributes.end()) {
    auto& uvAcc = ir_asset.accessors[uv->accessorIndex];
    fastgltf::iterateAccessorWithIndex<v2f>(
      ir_asset, uvAcc, [&](v2f v, size_t index) {
        mr_vtxs[i_initialVtx + index].uvX = v.x;
        mr_vtxs[i_initialVtx + index].uvY = v.y;
      });
  }
}

core::code
load_mesh_colors(fastgltf::Asset& ir_asset,
                 fastgltf::Primitive& ir_primitive,
                 u32 i_initialVtx,
                 std::vector<render::MeshVertex>& mr_vtxs)
{
  auto colors = ir_primitive.findAttribute("COLOR_0");
  if (colors != ir_primitive.attributes.end()) {
    auto& colorsAcc = ir_asset.accessors[colors->accessorIndex];
    fastgltf::iterateAccessorWithIndex<v4f>(
      ir_asset, colorsAcc, [&](v4f v, size_t index) {
        mr_vtxs[i_initialVtx + index].color = v;
      });
  }
}

core::code
load_mesh_materials(
  fastgltf::Asset& ir_asset,
  fastgltf::Primitive& ir_primitive,
  std::vector<std::shared_ptr<render::Material>>& ir_materials,
  u32 i_initialVtx,
  std::vector<render::MeshVertex>& mr_vtxs,
  render::MeshSurface& mr_newSurface)
{
  if (ir_primitive.materialIndex.has_value()) {
    mr_newSurface.p_material = ir_materials[ir_primitive.materialIndex.value()];
  } else {
    mr_newSurface.p_material = ir_materials[0];
  }
}

core::code
load_mesh_bounds(fastgltf::Asset& ir_asset,
                 u32 i_initialVtx,
                 std::vector<render::MeshVertex>& mr_vtxs,
                 render::MeshSurface& mr_newSurface)
{
  v3f minpos = mr_vtxs[i_initialVtx].position;
  v3f maxpos = mr_vtxs[i_initialVtx].position;
  for (int i = i_initialVtx; i < mr_vtxs.size(); i++) {
    minpos = glm::min(minpos, mr_vtxs[i].position);
    maxpos = glm::max(maxpos, mr_vtxs[i].position);
  }
  v3f extents = (maxpos - minpos) / 2.f;
  mr_newSurface.bounds = {
    .origin = (maxpos + minpos) / 2.f,
    .sphereRadius = glm::length(extents),
    .extents = extents,
  };
}

core::code
render::gltf::GLTFScene::init_meshes(
  fastgltf::Asset& ir_asset,
  std::vector<std::shared_ptr<render::Material>>& ir_materials,
  std::vector<std::shared_ptr<render::Mesh>>& mr_meshes)
{
  std::vector<u32> idxs;
  std::vector<render::MeshVertex> vtxs;
  for (auto& ir_mesh : ir_asset.meshes) {
    std::shared_ptr<render::Mesh> p_newMesh = std::make_shared<render::Mesh>();
    auto& newMesh = *(p_newMesh.get());
    for (auto&& it : ir_mesh.primitives) {
      render::MeshSurface newSurface;
      newSurface.startIdx = (u32)idxs.size();
      u32 initialVtx = vtxs.size();
      // load indices
      load_mesh_indices(ir_asset, it, initialVtx, idxs, newSurface);
      load_mesh_positions(ir_asset, it, initialVtx, vtxs);
      load_mesh_normals(ir_asset, it, initialVtx, vtxs);
      load_mesh_uvs(ir_asset, it, initialVtx, vtxs);
      load_mesh_colors(ir_asset, it, initialVtx, vtxs);
      load_mesh_materials(
        ir_asset, it, ir_materials, initialVtx, vtxs, newSurface);
      load_mesh_bounds(ir_asset, initialVtx, vtxs, newSurface);
      newMesh.surfaces.push_back(newSurface);
    }
    render::GPUMeshBuffers meshBuffers;
    auto status = upload_mesh(*p_vkMgr_, idxs, vtxs, meshBuffers);
    if (status != core::code::SUCCESS) {
      ERR("could not load gltf asset to gpu.");
      return core::code::ERROR;
    }
    newMesh.buffers = meshBuffers;
    newMesh.name = ir_mesh.name;
    meshes[newMesh.name.c_str()] = p_newMesh;
    mr_meshes.push_back(p_newMesh);
    vtxs.clear();
    idxs.clear();
  }
  return core::code::SUCCESS;
};

core::code
render::gltf::GLTFScene::init_nodes(
  fastgltf::Asset& ir_asset,
  std::vector<std::shared_ptr<render::Mesh>>& ir_meshes,
  std::vector<std::shared_ptr<render::Node>>& mr_nodes)
{
  for (fastgltf::Node& node : ir_asset.nodes) {
    std::shared_ptr<render::Node> newNode;
    if (node.meshIndex.has_value()) {
      newNode = std::make_shared<render::MeshNode>();
      static_cast<render::MeshNode*>(newNode.get())->p_mesh =
        ir_meshes[*node.meshIndex];
    } else {
      newNode = std::make_shared<render::Node>();
    }
    std::visit(
      fastgltf::visitor{
        [&](fastgltf::math::fmat4x4 matrix) {
          memcpy(&newNode->local, matrix.data(), sizeof(matrix));
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
          newNode->local = tm * rm * sm;
        },
        [&](auto& ir_other) {
          WRN("tried to load unimplemented transform format");
        } },
      node.transform);
    mr_nodes.push_back(newNode);
    nodes[node.name.c_str()] = newNode;
  }
  // set graph
  for (int i = 0; i < ir_asset.nodes.size(); i++) {
    fastgltf::Node& node = ir_asset.nodes[i];
    std::shared_ptr<render::Node>& sceneNode = mr_nodes[i];
    for (auto& c : node.children) {
      sceneNode->children.push_back(mr_nodes[c]);
      mr_nodes[c]->p_parent = sceneNode;
    }
  }
  for (auto& node : mr_nodes) {
    if (node->p_parent.lock() == nullptr) {
      root.push_back(node);
      node->refresh_transform(glm::mat4{ 1.f });
    }
  }
  return core::code::SUCCESS;
}

core::code
render::gltf::GLTFScene::init(render::DefaultResources& ir_defaultRes,
                              mgmt::vulkan::descriptor::Writer& ir_writer,
                              render::MaterialPassPipelines& ir_materialPipes)
{
  if (initialized == core::status::INITIALIZED) {
    return core::code::SUCCESS;
  }
  if (initialized == core::status::ERROR) {
    return core::code::IN_ERROR_STATE;
  }
  auto status = core::code::ERROR;
  // load file
  fastgltf::Asset asset;
  status = core::io::gltf::load(p_path_, &asset);
  if (status != core::code::SUCCESS) {
    ERR("failed to load gltf asset with code: {}", (u32)status);
    return core::code::ERROR;
  }
  status = init_pools(asset);
  if (status != core::code::SUCCESS) {
    ERR("failed to initialize pools");
    return core::code::ERROR;
  }
  status = init_samplers(asset);
  if (status != core::code::SUCCESS) {
    ERR("failed to initialize samplers");
    return core::code::ERROR;
  }
  std::vector<mgmt::vulkan::image::AllocatedImage> images;
  status = init_images(asset, ir_defaultRes.errorImg, images);
  if (status != core::code::SUCCESS) {
    ERR("failed to initialize images");
    return core::code::ERROR;
  }
  std::vector<std::shared_ptr<render::Material>> materials;
  status = init_materials(
    ir_writer, ir_materialPipes, ir_defaultRes, asset, images, materials);
  if (status != core::code::SUCCESS) {
    ERR("failed to initialize materials");
    return core::code::ERROR;
  }
  std::vector<std::shared_ptr<render::Mesh>> meshes;
  status = init_meshes(asset, materials, meshes);
  if (status != core::code::SUCCESS) {
    ERR("failed to initialize meshes");
    return core::code::ERROR;
  }
  std::vector<std::shared_ptr<render::Node>> nodes;
  status = init_nodes(asset, meshes, nodes);
  if (status != core::code::SUCCESS) {
    ERR("failed to initialize nodes");
    return core::code::ERROR;
  }
  initialized = core::status::INITIALIZED;
  return core::code::SUCCESS;
}

render::gltf::GLTFScene::GLTFScene(char* p_path, mgmt::vulkan::Manager* p_vkMgr)
  : p_path_{ p_path }
  , p_vkMgr_{ p_vkMgr } {};

//
// destructor
//

core::code
render::gltf::GLTFScene::destroy()
{
  if (initialized == core::status::NOT_INITIALIZED) {
    return core::code::SUCCESS;
  }
  if (initialized == core::status::ERROR) {
    return core::code::IN_ERROR_STATE;
  }
  auto& vkMgr = *p_vkMgr_;
  VkDevice dev = vkMgr.get_dev();
  descriptorPool.destroy_pools(dev);
  vkMgr.destroy_buffer(materialDataBuff);
  for (auto& [k, v] : meshes) {
    vkMgr.destroy_buffer(v->buffers.idxBuff);
    vkMgr.destroy_buffer(v->buffers.vertexBuff);
  }
  for (auto& sampler : samplers) {
    vkDestroySampler(dev, sampler, nullptr);
  }
  delQueue_.flush();
  initialized = core::status::NOT_INITIALIZED;
  return core::code::SUCCESS;
}

render::gltf::GLTFScene::~GLTFScene()
{
  destroy();
}

//
// draw
//

void
render::gltf::GLTFScene::Draw(const glm::mat4& top, DrawContext& ctx)
{
  for (auto& n : root) {
    n->Draw(top, ctx);
  }
};
