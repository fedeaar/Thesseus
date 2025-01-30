#pragma once

#include "../renderer.h"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

namespace RenderEngine {

class ImguiRenderer : Renderer
{
private:
  ResourceManagement::WindowManager* window_mgr_;

public:
  RenderEngine::Status init(
    ResourceManagement::VulkanManager::Swapchain::Swapchain& swapchain);
  ImguiRenderer(ResourceManagement::VulkanManager::Manager* vk_mgr,
                ResourceManagement::WindowManager* window_mgr);

  RenderEngine::Status destroy();
  ~ImguiRenderer();

  RenderEngine::Status draw(
    VkCommandBuffer cmd,
    u32 img_idx,
    ResourceManagement::VulkanManager::Swapchain::Swapchain& swapchain);
};
} // namespace RenderEngine
