#pragma once

#include "../render.h"
#include "../renderer/imgui-renderer/renderer.h"
#include "../renderer/swap-renderer/renderer.h"
#include "../renderer/triangle-mesh-renderer/renderer.h"
#include "../renderer/triangle-renderer/renderer.h"

namespace render {
class Engine
{
public:
  struct Params
  {
    u32 screen_width, screen_height;
    std::string name;
  };

  bool initialized = false;
  u32 frame = 0;

private:
  std::string const namespace_ = render::namespace_ + "::Engine";
  core::Logger logger_{ namespace_ };

  Params params_;
  mgmt::WindowManager window_mgr_;
  mgmt::vulkan::Manager vk_mgr_;
  mgmt::vulkan::swapchain::Swapchain swapchain_;

public:
  SwapRenderer swap_renderer_;
  ImguiRenderer imgui_renderer_;
  TriangleRenderer triangle_renderer_;
  TriangleMeshRenderer triangle_mesh_renderer_;

  core::Status init();
  Engine(Params& params);

  core::Status destroy();
  ~Engine();

  core::Status render();

  f32 get_aspect_ratio() { return window_mgr_.aspect_ratio; };
};
} // namespace render
