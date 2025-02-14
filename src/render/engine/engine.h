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

  struct State
  {
    core::status initialized = core::status::NOT_INIT;
    mgmt::window::Manager window_mgr;
    mgmt::vulkan::Manager vk_mgr;
    mgmt::vulkan::Swapchain swapchain;
  } state;

private:
  BackgroundRenderer background_renderer_;
  AssetRenderer asset_renderer_;
  ImguiRenderer imgui_renderer_;

public:
  core::code init();
  Engine(Params& params);

  core::code destroy();
  ~Engine();

  void render(Camera& camera);

  f32 get_aspect_ratio();
  f32& get_render_scale();
  mgmt::window::Manager* get_window_mgr();
  void maybe_resize_swapchain();
};

} // namespace render
