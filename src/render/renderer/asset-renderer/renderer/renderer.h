#pragma once

#include "../asset/asset.h"
#include "../material/material.h"
#include "../mesh/mesh.h"
#include "../renderer.h"

namespace render {

class AssetRenderer : Renderer
{
public:
  bool initialized = false;

private:
  // scene
  struct GPUSceneData
  {
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewproj;
    glm::vec4 ambient;
    glm::vec4 sun_dir;
    glm::vec4 sun_color;
  } scene_;
  VkDescriptorSetLayout scene_layout_;
  mgmt::vulkan::buffer::AllocatedBuffer scene_buffer_;
  mgmt::vulkan::descriptor::Writer writer_;
  // materials
  struct GPUMaterialConstants
  {
    v4f color_factors;
    v4f metal_rough_factors;
    // padding
    v4f extra[14];
  };
  struct MaterialResources
  {
    mgmt::vulkan::image::AllocatedImage color_img;
    mgmt::vulkan::image::AllocatedImage metal_img;
    VkSampler color_sampler;
    VkSampler metal_sampler;
    VkBuffer data_buff;
    u32 data_buff_offset;
  };
  asset::material::Pipeline opaque_pipeline_;
  asset::material::Pipeline transparent_pipeline_;
  VkDescriptorSetLayout material_layout_;
  // data
  std::vector<std::shared_ptr<asset::mesh::Mesh>> meshes_;
  std::unordered_map<std::string, std::shared_ptr<render::asset::Node>>
    loaded_nodes_;
  render::asset::DrawContext main_draw_ctx_;
  mgmt::vulkan::image::AllocatedImage white_img_;
  mgmt::vulkan::image::AllocatedImage black_img_;
  mgmt::vulkan::image::AllocatedImage gray_img_;
  mgmt::vulkan::image::AllocatedImage error_checker_img_;
  VkSampler default_linear_sampler_;
  VkSampler default_nearest_sampler_;
  asset::material::Instance default_material_data_;
  // mgmt
  DestructorQueue del_queue_;

public:
  core::code init(mgmt::vulkan::Swapchain& swapchain);
  AssetRenderer(mgmt::vulkan::Manager* vk_mgr);

  core::code destroy();
  ~AssetRenderer();

  void draw(mgmt::vulkan::Swapchain& swapchain);
  void update_scene(mgmt::vulkan::Swapchain& swapchain, Camera& camera);
  asset::material::Instance write_material(
    asset::material::Type pass,
    const MaterialResources& resources,
    mgmt::vulkan::descriptor::DynamicAllocator& descriptor_allocator);

private:
  core::code init_pipelines(mgmt::vulkan::Swapchain& swapchain);
  core::code init_meshes();
  core::code init_default_data();
  core::code init_constants();
  core::code init_scene();
};

} // namespace render
