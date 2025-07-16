#include "asset.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <variant>

//
// DefaultResources
//

core::code
render::asset::DefaultResources::init(
  mgmt::vulkan::descriptor::Writer& writer,
  render::asset::material::MaterialPipelines& material_pipes)
{
  if (initialized) {
    return core::code::SUCCESS;
  }
  // default imgs
  u32 white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
  if (vk_mgr_->create_image((void*)&white,
                            VkExtent3D{ 1, 1, 1 },
                            VK_FORMAT_R8G8B8A8_UNORM,
                            VK_IMAGE_USAGE_SAMPLED_BIT,
                            false,
                            white_img) != core::code::SUCCESS) {
    core::Logger::err("render::asset::DefaultResources::init",
                      "could not load white img");
    return core::code::ERROR;
  }
  u32 gray = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
  if (vk_mgr_->create_image((void*)&gray,
                            VkExtent3D{ 1, 1, 1 },
                            VK_FORMAT_R8G8B8A8_UNORM,
                            VK_IMAGE_USAGE_SAMPLED_BIT,
                            false,
                            gray_img) != core::code::SUCCESS) {
    core::Logger::err("render::asset::DefaultResources::init",
                      "could not load gray img");
    return core::code::ERROR;
  }
  u32 black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 0));
  if (vk_mgr_->create_image((void*)&black,
                            VkExtent3D{ 1, 1, 1 },
                            VK_FORMAT_R8G8B8A8_UNORM,
                            VK_IMAGE_USAGE_SAMPLED_BIT,
                            false,
                            black_img) != core::code::SUCCESS) {
    core::Logger::err("render::asset::DefaultResources::init",
                      "could not load black img");
    return core::code::ERROR;
  }
  u32 magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
  std::array<u32, 16 * 16> pixels;
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
    }
  }
  if (vk_mgr_->create_image(pixels.data(),
                            VkExtent3D{ 16, 16, 1 },
                            VK_FORMAT_R8G8B8A8_UNORM,
                            VK_IMAGE_USAGE_SAMPLED_BIT,
                            false,
                            error_checker_img) != core::code::SUCCESS) {
    core::Logger::err("render::asset::DefaultResources::init",
                      "could not load checker img");
    return core::code::ERROR;
  }
  // default samplers
  VkSamplerCreateInfo sampl = { .sType =
                                  VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
  sampl.magFilter = VK_FILTER_NEAREST;
  sampl.minFilter = VK_FILTER_NEAREST;
  vkCreateSampler(
    vk_mgr_->get_dev(), &sampl, nullptr, &default_nearest_sampler);
  sampl.magFilter = VK_FILTER_LINEAR;
  sampl.minFilter = VK_FILTER_LINEAR;
  vkCreateSampler(vk_mgr_->get_dev(), &sampl, nullptr, &default_linear_sampler);
  // default material textures
  asset::material::MaterialResources resources;
  resources.color_img = error_checker_img;
  resources.color_sampler = default_nearest_sampler;
  resources.metal_img = error_checker_img;
  resources.metal_sampler = default_nearest_sampler;
  mgmt::vulkan::buffer::AllocatedBuffer constants =
    vk_mgr_
      ->create_buffer(sizeof(asset::material::GPUMaterialConstants),
                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VMA_MEMORY_USAGE_CPU_TO_GPU)
      .value(); // TODO: error handling
  asset::material::GPUMaterialConstants scene_uniform_data;
  scene_uniform_data.color_factors = v4f{ 1, 1, 1, 1 };
  scene_uniform_data.metal_rough_factors = v4f{ 1, 0.5, 0, 0 };
  void* p_scene_uniform_data;
  vmaMapMemory(
    vk_mgr_->get_allocator(), constants.allocation, &p_scene_uniform_data);
  memcpy(p_scene_uniform_data,
         &scene_uniform_data,
         sizeof(asset::material::GPUMaterialConstants));
  vmaUnmapMemory(vk_mgr_->get_allocator(), constants.allocation);
  resources.data_buff = constants.buffer;
  resources.data_buff_offset = 0;
  default_material_data = render::asset::material::write_material(
    asset::material::Type::opaque,
    resources,
    *vk_mgr_,
    vk_mgr_->get_global_descriptor_allocator(),
    writer,
    material_pipes);
  // del
  del_queue_.push([=, this]() {
    auto dev = vk_mgr_->get_dev();
    vk_mgr_->destroy_buffer(constants);
    vkDestroySampler(dev, default_linear_sampler, nullptr);
    vkDestroySampler(dev, default_nearest_sampler, nullptr);
  });
  initialized = true;
  return core::code::SUCCESS;
}

render::asset::DefaultResources::DefaultResources(mgmt::vulkan::Manager* vk_mgr)
  : vk_mgr_{ vk_mgr }
{
}

core::code
render::asset::DefaultResources::destroy()
{
  del_queue_.flush();
  initialized = false;
  return core::code::SUCCESS;
}

render::asset::DefaultResources::~DefaultResources()
{
  if (initialized) {
    destroy();
  }
}

//
// Node
//

void
render::asset::Node::refresh_transform(const m4f& parent)
{
  world_tf = parent * local_tf;
  for (auto c : children) {
    c->refresh_transform(world_tf);
  }
}

void
render::asset::Node::Draw(const m4f& top, DrawContext& ctx)
{
  // draw children
  for (auto& c : children) {
    c->Draw(top, ctx);
  }
}

//
// MeshNode
//

void
render::asset::MeshNode::Draw(const m4f& top, render::asset::DrawContext& ctx)
{
  m4f node_matrix = top * world_tf;
  for (auto& s : mesh->surfaces) {
    Object def;
    def.idx_count = s.count;
    def.start_idx = s.start_idx;
    def.idx_buff = mesh->mesh_buffers.index_buff.buffer;
    def.material = &s.material->data;
    def.transform = node_matrix;
    def.vertex_buff_addr = mesh->mesh_buffers.vertex_buff_addr;
    ctx.opaque_surfaces.push_back(def);
  }
  Node::Draw(top, ctx);
}

//
// gltf
//

core::code
render::asset::load_gltf_asset(
  mgmt::vulkan::Manager& vk_mgr,
  char* path,
  render::asset::DefaultResources& default_res,
  mgmt::vulkan::descriptor::Writer& writer,
  render::asset::material::MaterialPipelines& material_pipes,
  std::shared_ptr<render::asset::LoadedGLTF>& scene)
{
  VkDevice dev = vk_mgr.get_dev();
  // scene
  scene->vk_mgr = &vk_mgr;
  LoadedGLTF& file = *scene.get();
  // load file
  fastgltf::Asset gltf;
  if (core::io::gltf::load(path, &gltf) != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::load_gltf_meshes",
                      "could not load gltf asset");
    return core::code::ERROR;
  }
  // pool
  std::vector<mgmt::vulkan::descriptor::PoolSizeRatio> sizes = {
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 }
  };
  file.descriptor_pool.init(vk_mgr.get_dev(), gltf.materials.size(), sizes);
  // samplers
  for (fastgltf::Sampler& sampler : gltf.samplers) {
    VkSamplerCreateInfo sampl = { .sType =
                                    VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                                  .pNext = nullptr };
    sampl.maxLod = VK_LOD_CLAMP_NONE;
    sampl.minLod = 0;
    sampl.magFilter = core::io::gltf::to_vulkan_filter(
      sampler.magFilter.value_or(fastgltf::Filter::Nearest));
    sampl.minFilter = core::io::gltf::to_vulkan_filter(
      sampler.minFilter.value_or(fastgltf::Filter::Nearest));
    sampl.mipmapMode = core::io::gltf::to_vulkan_mipmap_mode(
      sampler.minFilter.value_or(fastgltf::Filter::Nearest));
    VkSampler newSampler;
    vkCreateSampler(dev, &sampl, nullptr, &newSampler);
    file.samplers.push_back(newSampler);
  }
  // temporal arrays for all the objects to use while creating the GLTF data
  std::vector<std::shared_ptr<render::asset::mesh::Mesh>> meshes;
  std::vector<std::shared_ptr<Node>> nodes;
  std::vector<mgmt::vulkan::image::AllocatedImage> images;
  std::vector<std::shared_ptr<render::asset::material::Material>> materials;
  // load all textures
  for (fastgltf::Image& image : gltf.images) {
    images.push_back(default_res.error_checker_img);
  }
  // create buffer to hold the material data
  file.material_data_buff =
    vk_mgr
      .create_buffer(sizeof(asset::material::GPUMaterialConstants) *
                       gltf.materials.size(),
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VMA_MEMORY_USAGE_CPU_TO_GPU)
      .value(); // TODO
  int data_index = 0;
  asset::material::GPUMaterialConstants* scene_material_constants =
    (asset::material::GPUMaterialConstants*)
      file.material_data_buff.info.pMappedData;
  for (fastgltf::Material& mat : gltf.materials) {
    std::shared_ptr<asset::material::Material> newMat =
      std::make_shared<asset::material::Material>();
    materials.push_back(newMat);
    file.materials[mat.name.c_str()] = newMat;
    asset::material::GPUMaterialConstants constants;
    constants.color_factors.x = mat.pbrData.baseColorFactor[0];
    constants.color_factors.y = mat.pbrData.baseColorFactor[1];
    constants.color_factors.z = mat.pbrData.baseColorFactor[2];
    constants.color_factors.w = mat.pbrData.baseColorFactor[3];
    constants.metal_rough_factors.x = mat.pbrData.metallicFactor;
    constants.metal_rough_factors.y = mat.pbrData.roughnessFactor;
    // write material parameters to buffer
    scene_material_constants[data_index] = constants;

    asset::material::Type passType = asset::material::Type::opaque;
    if (mat.alphaMode == fastgltf::AlphaMode::Blend) {
      passType = asset::material::Type::transparent;
    }
    asset::material::MaterialResources resources;
    // default the material textures
    resources.color_img = default_res.white_img;
    resources.color_sampler = default_res.default_linear_sampler;
    resources.metal_img = default_res.white_img;
    resources.metal_sampler = default_res.default_linear_sampler;
    // set the uniform buffer for the material data
    resources.data_buff = file.material_data_buff.buffer;
    resources.data_buff_offset =
      data_index * sizeof(asset::material::MaterialResources);
    // grab textures from gltf file
    if (mat.pbrData.baseColorTexture.has_value()) {
      size_t img =
        gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex]
          .imageIndex.value();
      size_t sampler =
        gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex]
          .samplerIndex.value();

      resources.color_img = images[img];
      resources.color_sampler = file.samplers[sampler];
    }
    // build material
    newMat->data = render::asset::material::write_material(passType,
                                                           resources,
                                                           vk_mgr,
                                                           file.descriptor_pool,
                                                           writer,
                                                           material_pipes);
    data_index++;
  }
  // use the same vectors for all meshes so that the memory doesnt reallocate as
  // often
  std::vector<u32> indices;
  std::vector<render::asset::mesh::Vertex> vertices;
  for (fastgltf::Mesh& mesh : gltf.meshes) {
    std::shared_ptr<render::asset::mesh::Mesh> newmesh =
      std::make_shared<render::asset::mesh::Mesh>();
    meshes.push_back(newmesh);
    file.meshes[mesh.name.c_str()] = newmesh;
    newmesh->name = mesh.name;
    // clear the mesh arrays each mesh, we dont want to merge them by error
    indices.clear();
    vertices.clear();
    for (auto&& p : mesh.primitives) {
      render::asset::mesh::Surface newSurface;
      newSurface.start_idx = (uint32_t)indices.size();
      newSurface.count =
        (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;
      size_t initial_vtx = vertices.size();
      // load indexes
      {
        fastgltf::Accessor& indexaccessor =
          gltf.accessors[p.indicesAccessor.value()];
        indices.reserve(indices.size() + indexaccessor.count);

        fastgltf::iterateAccessor<std::uint32_t>(
          gltf, indexaccessor, [&](std::uint32_t idx) {
            indices.push_back(idx + initial_vtx);
          });
      }
      // load vertex positions
      {
        fastgltf::Accessor& posAccessor =
          gltf.accessors[p.findAttribute("POSITION")->accessorIndex];
        vertices.resize(vertices.size() + posAccessor.count);
        fastgltf::iterateAccessorWithIndex<glm::vec3>(
          gltf, posAccessor, [&](glm::vec3 v, size_t index) {
            asset::mesh::Vertex newvtx;
            newvtx.position = v;
            newvtx.normal = { 1, 0, 0 };
            newvtx.color = glm::vec4{ 1.f };
            newvtx.uv_x = 0;
            newvtx.uv_y = 0;
            vertices[initial_vtx + index] = newvtx;
          });
      }
      // load vertex normals
      auto normals = p.findAttribute("NORMAL");
      if (normals != p.attributes.end()) {

        fastgltf::iterateAccessorWithIndex<glm::vec3>(
          gltf,
          gltf.accessors[(*normals).accessorIndex],
          [&](glm::vec3 v, size_t index) {
            vertices[initial_vtx + index].normal = v;
          });
      }
      // load UVs
      auto uv = p.findAttribute("TEXCOORD_0");
      if (uv != p.attributes.end()) {

        fastgltf::iterateAccessorWithIndex<glm::vec2>(
          gltf,
          gltf.accessors[(*uv).accessorIndex],
          [&](glm::vec2 v, size_t index) {
            vertices[initial_vtx + index].uv_x = v.x;
            vertices[initial_vtx + index].uv_y = v.y;
          });
      }
      // load vertex colors
      auto colors = p.findAttribute("COLOR_0");
      if (colors != p.attributes.end()) {
        fastgltf::iterateAccessorWithIndex<glm::vec4>(
          gltf,
          gltf.accessors[(*colors).accessorIndex],
          [&](glm::vec4 v, size_t index) {
            vertices[initial_vtx + index].color = v;
          });
      }
      if (p.materialIndex.has_value()) {
        newSurface.material = materials[p.materialIndex.value()];
      } else {
        newSurface.material = materials[0];
      }
      newmesh->surfaces.push_back(newSurface);
    }
    newmesh->mesh_buffers =
      asset::mesh::upload_mesh(vk_mgr, indices, vertices).value(); // todo
  }
  // load all nodes and their meshes
  for (fastgltf::Node& node : gltf.nodes) {
    std::shared_ptr<Node> newNode;
    // find if the node has a mesh, and if it does hook it to the mesh pointer
    // and allocate it with the meshnode class
    if (node.meshIndex.has_value()) {
      newNode = std::make_shared<MeshNode>();
      static_cast<MeshNode*>(newNode.get())->mesh = meshes[*node.meshIndex];
    } else {
      newNode = std::make_shared<Node>();
    }
    nodes.push_back(newNode);
    file.nodes[node.name.c_str()];
    std::visit(fastgltf::visitor{
                 [&](fastgltf::math::fmat4x4 matrix) {
                   memcpy(&newNode->local_tf, matrix.data(), sizeof(matrix));
                 },
                 [&](fastgltf::TRS transform) {
                   glm::vec3 tl(transform.translation[0],
                                transform.translation[1],
                                transform.translation[2]);
                   glm::quat rot(transform.rotation[3],
                                 transform.rotation[0],
                                 transform.rotation[1],
                                 transform.rotation[2]);
                   glm::vec3 sc(transform.scale[0],
                                transform.scale[1],
                                transform.scale[2]);

                   glm::mat4 tm = glm::translate(glm::mat4(1.f), tl);
                   glm::mat4 rm = glm::toMat4(rot);
                   glm::mat4 sm = glm::scale(glm::mat4(1.f), sc);

                   newNode->local_tf = tm * rm * sm;
                 } },
               node.transform);
  }
  // run loop again to setup transform hierarchy
  for (int i = 0; i < gltf.nodes.size(); i++) {
    fastgltf::Node& node = gltf.nodes[i];
    std::shared_ptr<Node>& sceneNode = nodes[i];

    for (auto& c : node.children) {
      sceneNode->children.push_back(nodes[c]);
      nodes[c]->parent = sceneNode;
    }
  }
  // find the top nodes, with no parents
  for (auto& node : nodes) {
    if (node->parent.lock() == nullptr) {
      file.top_nodes.push_back(node);
      node->refresh_transform(glm::mat4{ 1.f });
    }
  }
  return core::code::SUCCESS;
};

void
render::asset::LoadedGLTF::Draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
  // create renderables from the scenenodes
  for (auto& n : top_nodes) {
    n->Draw(topMatrix, ctx);
  }
};
