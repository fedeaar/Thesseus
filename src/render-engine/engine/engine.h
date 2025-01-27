#pragma once

#include "../render-engine.h"

namespace RenderEngine {
class Engine
{
public:
  std::string const& name;
  struct Params
  {
    u32 screen_width, screen_height;
    string name;
  } params_;
  bool initialized_ = false;

  // WindowManager window_mgr_;
  // VulkanManager vk_mgr_;
  // Swapchain swapchain_;
  // std::vector<Renderer> renderers_;
  // DestructorQueue del_queue_;
  //
  Status init();
  Engine(Params& params);

  Status destroy();
  ~Engine();
  //
  core::Result<WindowManager&, Status> get_window_mgr();
  // core::Result<VulkanManager&, Status> get_vk_mgr();
  // core::Result<Swapchain&, Status> get_swapchain();
  //
  // Status render();
};
};
