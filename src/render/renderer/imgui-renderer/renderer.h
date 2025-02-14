#pragma once

#include "../renderer.h"

namespace render {

class ImguiRenderer : Renderer
{
private:
  mgmt::window::Manager* window_mgr_;
  VkDescriptorPool imgui_pool_;

public:
  core::code init(mgmt::vulkan::Swapchain& swapchain);
  ImguiRenderer(mgmt::vulkan::Manager* vk_mgr,
                mgmt::window::Manager* window_mgr);

  core::code destroy();
  ~ImguiRenderer();

  void draw(mgmt::vulkan::Swapchain& swapchain);
};

} // namespace render
