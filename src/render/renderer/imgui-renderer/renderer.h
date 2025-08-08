#pragma once

#include "../renderer.h"

namespace render {

class ImguiRenderer : Renderer
{
private:
  mgmt::window::Manager* p_windowMgr_;
  VkDescriptorPool imguiPool_;

public:
  core::code init();
  ImguiRenderer(mgmt::vulkan::Swapchain* mp_swapchain,
                mgmt::vulkan::Manager* mp_vkMgr,
                mgmt::window::Manager* mp_windowMgr,
                debug::GlobalStats* p_stats);

  core::code destroy();
  ~ImguiRenderer();

  void draw();
};

} // namespace render
