#pragma once

#include "../renderer.h"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

namespace render {

class ImguiRenderer : Renderer
{
private:
  mgmt::WindowManager* window_mgr_;

public:
  core::Status init(mgmt::vulkan::swapchain::Swapchain& swapchain);
  ImguiRenderer(mgmt::vulkan::Manager* vk_mgr, mgmt::WindowManager* window_mgr);

  core::Status destroy();
  ~ImguiRenderer();

  core::Status draw(VkCommandBuffer cmd,
                    u32 img_idx,
                    mgmt::vulkan::swapchain::Swapchain& swapchain);
};
} // namespace render
