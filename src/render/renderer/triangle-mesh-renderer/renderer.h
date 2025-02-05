#pragma once

#include "../renderer.h"

namespace render {

class TriangleMeshRenderer : Renderer
{
public:
  bool initialized = false;

private:
  DestructorQueue del_queue_;
  mgmt::vulkan::pipeline::Pipeline pipeline_;
  mgmt::vulkan::mesh::GPUMeshBuffers rectangle_;

public:
  core::Status init(mgmt::vulkan::swapchain::Swapchain& swapchain);
  TriangleMeshRenderer(mgmt::vulkan::Manager* vk_mgr);

  core::Status destroy();
  ~TriangleMeshRenderer();

  core::Status draw(VkCommandBuffer cmd,
                    u32 img_idx,
                    mgmt::vulkan::swapchain::Swapchain& swapchain);
};
} // namespace render
