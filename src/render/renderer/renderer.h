#pragma once

#include "../render.h"

namespace render {

class Renderer
{
public:
  core::status initialized = core::status::NOT_INITIALIZED;

protected:
  mgmt::vulkan::Manager* p_vkMgr_;
  mgmt::vulkan::Swapchain* p_swapchain_;

public:
  core::code init();
  Renderer(mgmt::vulkan::Manager* mp_vkMgr,
           mgmt::vulkan::Swapchain* mp_swapchain);

  core::code destroy();
  ~Renderer();

  void draw();
};

} // namespace render
