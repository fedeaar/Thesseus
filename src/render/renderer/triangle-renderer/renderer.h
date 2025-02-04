#pragma once

#include "../renderer.h"

namespace render {

class TriangleRenderer : Renderer
{
public:
  bool initialized = false;

private:
  mgmt::vulkan::pipeline::Pipeline pipeline_;

public:
  core::Status init(mgmt::vulkan::swapchain::Swapchain& swapchain);
  TriangleRenderer(mgmt::vulkan::Manager* vk_mgr);

  core::Status destroy();
  ~TriangleRenderer();

  core::Status draw(VkCommandBuffer cmd,
                    u32 img_idx,
                    mgmt::vulkan::swapchain::Swapchain& swapchain);
};
} // namespace render
