#pragma once

#include "../render.h"

namespace render {

class Renderer
{
public:
  bool initialized = false;

protected:
  std::string const namespace_ = render::namespace_ + "::Renderer";
  core::Logger logger_{ namespace_ };

  mgmt::vulkan::Manager* vk_mgr_;
  mgmt::vulkan::Swapchain::Swapchain swapchain_;

public:
  core::Status init() {};
  Renderer(mgmt::vulkan::Manager* vk_mgr)
    : vk_mgr_{ vk_mgr } {};

  core::Status destroy() {}; // todo@engine
  ~Renderer() {};

  core::Status draw(VkCommandBuffer cmd,
                    u32 img_idx,
                    mgmt::vulkan::Swapchain::Swapchain& swapchain)
  {
    throw "notImplemented";
  };
};
} // namespace render
