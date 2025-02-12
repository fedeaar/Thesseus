#pragma once

#include "../render.h"
#include "../renderer/custom-mesh-renderer/renderer.h"
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
  f32 render_scale = 1.f;

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
  CustomMeshRenderer custom_mesh_renderer_;

  core::Status init();
  Engine(Params& params);

  core::Status destroy();
  ~Engine();

  core::Status render(Camera& camera);

  f32 get_aspect_ratio() { return window_mgr_.aspect_ratio; };
  mgmt::WindowManager* get_window_mgr() { return &window_mgr_; };
  void maybe_resize_swapchain()
  {
    if (vk_mgr_.resize_requested) { // move
      vk_mgr_.resize_swapchain(swapchain_);
    }
  }
};
} // namespace render
