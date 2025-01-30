#pragma once

#include "../render-engine.h"

namespace RenderEngine {

class Renderer
{
public:
  bool initialized = false;

protected:
  std::string const namespace_ = RenderEngine::namespace_ + "::Renderer";
  core::Logger logger_{ namespace_ };

  ResourceManagement::VulkanManager::Manager* vk_mgr_;
  ResourceManagement::VulkanManager::Swapchain::Swapchain swapchain_;

public:
  RenderEngine::Status init() {};
  Renderer(ResourceManagement::VulkanManager::Manager* vk_mgr)
    : vk_mgr_{ vk_mgr } {};

  RenderEngine::Status destroy() {}; // todo@engine
  ~Renderer() {};

  RenderEngine::Status draw(
    VkCommandBuffer cmd,
    u32 img_idx,
    ResourceManagement::VulkanManager::Swapchain::Swapchain& swapchain)
  {
    throw "notImplemented";
  };
};
} // namespace RenderEngine
