#pragma once

#include "../../base/base.h"

namespace render {

class Renderer
{
public:
  bool initialized = false;

protected:
  std::string const namespace_ = render::namespace_ + "::Renderer";
  core::Logger logger_{ namespace_ };

  mgmt::vulkan::Manager* vk_mgr_;

public:
  core::Status init();
  Renderer(mgmt::vulkan::Manager* vk_mgr);

  core::Status destroy();
  ~Renderer();

  core::Status draw(VkCommandBuffer cmd,
                    mgmt::vulkan::swapchain::Swapchain& swapchain);
};
} // namespace render
