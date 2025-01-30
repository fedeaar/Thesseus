#pragma once

#include "../render-engine.h"
#include "../renderer/imgui-renderer/imgui-renderer.h"
#include "../renderer/swap-renderer/swap-renderer.h"

namespace RenderEngine {
class Engine
{
public:
  struct Params
  {
    u32 screen_width, screen_height;
    string name;
  };

  bool initialized = false;
  u32 frame = 0;

private:
  std::string const namespace_ = RenderEngine::namespace_ + "::Engine";
  core::Logger logger_{ namespace_ };

  Params params_;
  ResourceManagement::WindowManager window_mgr_;
  ResourceManagement::VulkanManager::Manager vk_mgr_;
  ResourceManagement::VulkanManager::Swapchain::Swapchain swapchain_;

public:
  SwapRenderer swap_renderer_;
  ImguiRenderer imgui_renderer_;

  RenderEngine::Status init();
  Engine(Params& params);

  RenderEngine::Status destroy();
  ~Engine();

  RenderEngine::Status render();

  f32 get_aspect_ratio() { return window_mgr_.aspect_ratio; };
};
} // namespace RenderEngine
