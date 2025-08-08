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
  debug::GlobalStats* p_stats_;

public:
  core::code init();
  Renderer(mgmt::vulkan::Manager* mp_vkMgr,
           mgmt::vulkan::Swapchain* mp_swapchain,
           debug::GlobalStats* mp_stats);

  core::code destroy();
  ~Renderer();

  void draw();
};

} // namespace render
