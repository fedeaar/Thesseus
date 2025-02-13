#pragma once

#include "../base/base.h"
#include "../renderer/renderer.h"

namespace render {
class Engine
{
public:
  enum Status
  {
    NOT_INIT = 0,
    INIT = 1,
    ERROR = 2
  };

  struct Params
  {
    u32 screen_width, screen_height;
    std::string name;
  };

  Status initialized = NOT_INIT;

private:
  std::string const namespace_ = render::namespace_ + "::Engine";
  core::Logger logger_{ namespace_ };

  mgmt::WindowManager window_mgr_;
  mgmt::vulkan::Manager vk_mgr_;
  mgmt::vulkan::swapchain::Swapchain swapchain_;

public:
  BackgroundRenderer background_renderer_;
  AssetRenderer asset_renderer_;
  ImguiRenderer imgui_renderer_;

  core::Status init();
  Engine(Params& params);

  core::Status destroy();
  ~Engine();

  core::Status render(Camera& camera);

  f32 get_aspect_ratio();
  f32& get_render_scale();
  mgmt::WindowManager* get_window_mgr();
  void maybe_resize_swapchain();
};

} // namespace render
