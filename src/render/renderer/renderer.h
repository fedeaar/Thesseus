#pragma once

#include "../render.h"

namespace render {

class Renderer
{
public:
  core::status initialized = core::status::NOT_INIT;

protected:
  mgmt::vulkan::Manager* vk_mgr_;

public:
  core::code init();
  Renderer(mgmt::vulkan::Manager* vk_mgr);

  core::code destroy();
  ~Renderer();

  void draw(mgmt::vulkan::swapchain::Swapchain& swapchain);
};

} // namespace render
