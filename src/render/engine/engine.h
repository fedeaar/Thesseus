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
    core::status initialized = core::status::NOT_INITIALIZED;
    mgmt::window::Manager window_mgr;
    mgmt::vulkan::Manager vk_mgr;
    mgmt::vulkan::Swapchain swapchain;
    // renderers
    BackgroundRenderer bg_renderer;
    AssetRenderer mesh_renderer;
    ImguiRenderer imgui_renderer;
  } state;

public:
  core::code init();
  Engine(Params& params);

  core::code destroy();
  ~Engine();

  void render(Camera& camera);
};

} // namespace render
