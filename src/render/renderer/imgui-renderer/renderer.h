#pragma once

#include "../renderer.h"

namespace render {

class ImguiRenderer : Renderer
{
private:
  mgmt::WindowManager* window_mgr_;

public:
  core::code init(mgmt::vulkan::swapchain::Swapchain& swapchain);
  ImguiRenderer(mgmt::vulkan::Manager* vk_mgr, mgmt::WindowManager* window_mgr);

  core::code destroy();
  ~ImguiRenderer();

  core::code draw(VkCommandBuffer cmd,
                  mgmt::vulkan::swapchain::Swapchain& swapchain);
};
} // namespace render
