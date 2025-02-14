#pragma once

#include "../render.h"
#include "../renderer/include.h"

namespace render {
class Engine
{
public:
  struct Params
  {
    u32 screen_width, screen_height;
    std::string name;
  };

  core::status initialized = core::status::NOT_INIT;

private:
  mgmt::WindowManager window_mgr_;
  mgmt::vulkan::Manager vk_mgr_;
  mgmt::vulkan::swapchain::Swapchain swapchain_;

public:
  BackgroundRenderer background_renderer_;
  AssetRenderer asset_renderer_;
  ImguiRenderer imgui_renderer_;

  core::code init();
  Engine(Params& params);

  core::code destroy();
  ~Engine();

  void render(Camera& camera);

  f32 get_aspect_ratio();
  f32& get_render_scale();
  mgmt::WindowManager* get_window_mgr();
  void maybe_resize_swapchain();
};

} // namespace render
