#pragma once

#include "../asset/asset.h"
#include "../gltf/gltf.h"
#include "../material/material.h"
#include "../mesh/mesh.h"
#include "../renderer.h"

namespace render {

class AssetRenderer : Renderer
{
public:
  core::status initialized = core::status::NOT_INITIALIZED;

private:
  // scene
  struct GPUSceneData
  {
    m4f view;
    m4f proj;
    m4f viewproj;
    v4f ambient;
    v4f sun_dir;
    v4f sun_color;
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
  core::code init();
  AssetRenderer(mgmt::vulkan::Swapchain* mp_swapchain,
                mgmt::vulkan::Manager* mp_vkMgr);

  core::code destroy();
  ~AssetRenderer();

  void draw(Camera& camera);
  void update_scene(Camera& camera);

private:
  core::code init_pipelines();
  core::code init_default_data();
  core::code init_meshes();
  core::code init_scene();
};

} // namespace render
