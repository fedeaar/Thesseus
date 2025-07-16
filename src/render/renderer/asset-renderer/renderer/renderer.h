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
  // pipelines
  asset::material::MaterialPipelines material_pipes_;
  // data
  std::vector<std::shared_ptr<asset::mesh::Mesh>> meshes_;
  std::unordered_map<std::string, std::shared_ptr<render::asset::Node>>
    loaded_nodes_;
  render::asset::DrawContext main_draw_ctx_;
  std::unordered_map<std::string, std::shared_ptr<asset::LoadedGLTF>>
    loaded_scenes_;
  asset::DefaultResources default_res_;
  // mgmt
  DestructorQueue del_queue_;

public:
  core::code init(mgmt::vulkan::Swapchain& swapchain);
  AssetRenderer(mgmt::vulkan::Manager* vk_mgr);

  core::code destroy();
  ~AssetRenderer();

  void draw(mgmt::vulkan::Swapchain& swapchain, Camera& camera);
  void update_scene(mgmt::vulkan::Swapchain& swapchain, Camera& camera);

private:
  core::code init_pipelines(mgmt::vulkan::Swapchain& swapchain);
  core::code init_meshes();
  core::code init_default_data();
  core::code init_scene();
};

} // namespace render
