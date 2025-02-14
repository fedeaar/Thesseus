#pragma once

#include "../renderer.h"
#include <memory>

namespace render {

class AssetRenderer : Renderer
{
public:
  bool initialized = false;

private:
  DestructorQueue del_queue_;
  mgmt::vulkan::pipeline::Pipeline pipeline_;
  std::vector<std::shared_ptr<mgmt::vulkan::mesh::MeshAsset>> meshes_;
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
  VkDescriptorSetLayout single_img_layout_;
  mgmt::vulkan::image::AllocatedImage white_img_;
  mgmt::vulkan::image::AllocatedImage black_img_;
  mgmt::vulkan::image::AllocatedImage gray_img_;
  mgmt::vulkan::image::AllocatedImage error_checker_img_;
  VkSampler default_linear_sampler_;
  VkSampler default_nearest_sampler_;

public:
  core::code init(mgmt::vulkan::swapchain::Swapchain& swapchain);
  AssetRenderer(mgmt::vulkan::Manager* vk_mgr);

  core::code destroy();
  ~AssetRenderer();

  void draw(mgmt::vulkan::swapchain::Swapchain& swapchain, Camera& camera);
};

} // namespace render
