#include "asset.h"

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
    if (s.material->data.type == material::Type::opaque) {
      ctx.opaque_surfaces.push_back(def);
    } else {
      ctx.transparent_surfaces.push_back(def);
    }
  }
  Node::Draw(top, ctx);
}

void
render::asset::LoadedGLTF::Draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
  // create renderables from the scenenodes
  for (auto& n : top_nodes) {
    n->Draw(topMatrix, ctx);
  }
};

void
render::asset::LoadedGLTF::clearAll()
{
  if (!initialized) {
    return;
  }
  initialized = false;
  VkDevice dev = vk_mgr->get_dev();
  descriptor_pool.destroy_pools(dev);
  vk_mgr->destroy_buffer(material_data_buff);
  for (auto& [k, v] : meshes) {
    vk_mgr->destroy_buffer(v->mesh_buffers.index_buff);
    vk_mgr->destroy_buffer(v->mesh_buffers.vertex_buff);
  }
  for (auto& sampler : samplers) {
    vkDestroySampler(dev, sampler, nullptr);
  }
  del_queue_.flush();
}
