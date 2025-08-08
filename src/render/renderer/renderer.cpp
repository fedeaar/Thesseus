#pragma once

#include "renderer.h"

core::code
render::Renderer::init()
{
  ERR("base renderer called");
  return core::code::NOT_IMPLEMENTED;
};

render::Renderer::Renderer(mgmt::vulkan::Manager* mp_vkMgr,
                           mgmt::vulkan::Swapchain* mp_swapchain,
                           debug::GlobalStats* mp_stats)
  : p_vkMgr_{ mp_vkMgr }
  , p_swapchain_{ mp_swapchain }
  , p_stats_{ mp_stats } {};

core::code
render::Renderer::destroy()
{
  ERR("base renderer called");
  return core::code::NOT_IMPLEMENTED;
};

render::Renderer::~Renderer() {};

void
render::Renderer::draw()
{
  ERR("base renderer called");
};
