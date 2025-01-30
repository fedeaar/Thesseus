#pragma once

#include "../renderer.h"

namespace RenderEngine {

class SwapRenderer : Renderer
{
public:
  struct ComputePushConstants
  {
    glm::vec4 data1;
    glm::vec4 data2;
    glm::vec4 data3;
    glm::vec4 data4;
  };
  struct ComputeEffect
  {
    const char* name;

    ResourceManagement::VulkanManager::Pipeline::Pipeline pipeline;
    ComputePushConstants data;
  };

  bool initialized = false;

  std::vector<ComputeEffect> effects_;
  u32 current_effect_;

  RenderEngine::Status init(
    ResourceManagement::VulkanManager::Swapchain::Swapchain& swapchain);
  SwapRenderer(ResourceManagement::VulkanManager::Manager* vk_mgr);

  RenderEngine::Status destroy();
  ~SwapRenderer();

  RenderEngine::Status draw(
    VkCommandBuffer cmd,
    u32 img_idx,
    ResourceManagement::VulkanManager::Swapchain::Swapchain& swapchain);
};
} // namespace RenderEngine
