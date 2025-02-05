#pragma once

#include "../renderer.h"
#include <memory>

namespace render {

class CustomMeshRenderer : Renderer
{
public:
  bool initialized = false;

private:
  DestructorQueue del_queue_;
  mgmt::vulkan::pipeline::Pipeline pipeline_;
  std::vector<std::shared_ptr<mgmt::vulkan::mesh::MeshAsset>> meshes_;

public:
  core::Status init(mgmt::vulkan::swapchain::Swapchain& swapchain);
  CustomMeshRenderer(mgmt::vulkan::Manager* vk_mgr);

  core::Status destroy();
  ~CustomMeshRenderer();

  core::Status draw(VkCommandBuffer cmd,
                    u32 img_idx,
                    mgmt::vulkan::swapchain::Swapchain& swapchain);
};
} // namespace render
