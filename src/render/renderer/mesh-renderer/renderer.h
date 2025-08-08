#pragma once

#include "../renderer.h"
#include "./gltf/gltf.h"
#include "./types.h"

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
    v4f sunDir;
    v4f sunColor;
  } sceneData_;
  VkDescriptorSetLayout sceneLayout_;
  mgmt::vulkan::buffer::AllocatedBuffer sceneBuffer_;
  // pipelines
  MaterialPassPipelines materialPipes_;
  // data
  DrawContext mainDrawCtx_;
  std::unordered_map<std::string, std::shared_ptr<gltf::GLTFScene>>
    loadedScenes_;
  DefaultResources defaultRes_;
  // mgmt
  mgmt::vulkan::descriptor::Writer writer_;
  DestructorQueue delQueue_;

public:
  core::code init();
  AssetRenderer(mgmt::vulkan::Swapchain* p_swapchain,
                mgmt::vulkan::Manager* p_vkMgr,
                debug::GlobalStats* p_stats);

  core::code destroy();
  ~AssetRenderer();

  void draw(Camera& camera);

private:
  core::code init_pipelines();
  core::code init_default_data();
  core::code init_scenes();

  void update_scene(Camera& camera);
};

} // namespace render
