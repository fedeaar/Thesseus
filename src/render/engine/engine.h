#pragma once

#include "../render.h"
#include "../renderer/include.h"

namespace render {

class Engine
{
public:
  struct Params
  {
    u32 screenWidth, screenHeight;
    std::string name;
  };

  struct State
  {
    core::status initialized = core::status::NOT_INITIALIZED;
    mgmt::window::Manager windowMgr;
    mgmt::vulkan::Manager vkMgr;
    mgmt::vulkan::Swapchain swapchain;
    // renderers
    BackgroundRenderer bgRenderer;
    AssetRenderer meshRenderer;
    ImguiRenderer imguiRenderer;
  } state;

public:
  core::code init();
  Engine(Params& params);

  core::code destroy();
  ~Engine();

  void render(Camera& camera);
};

} // namespace render
