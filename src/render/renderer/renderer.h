#pragma once

#include "../render.h"

namespace render {

class Renderer
{
public:
  core::status initialized = core::status::NOT_INITIALIZED;

protected:
  mgmt::vulkan::Manager* vk_mgr_;

public:
  core::code init();
  Renderer(mgmt::vulkan::Manager* vk_mgr);

  core::code destroy();
  ~Renderer();

  void draw(mgmt::vulkan::Swapchain& swapchain);
};

} // namespace render
