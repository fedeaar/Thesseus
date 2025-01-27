#pragma once

#include "../render-engine.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace RenderEngine {

class WindowManager
{
public:
  const std::string& name = namespace_() + "::WindowManager";
  bool initialized = false;
  u32 width;
  u32 height;
  f32 aspect_ratio;

private:
  core::Logger logger_{ name };

  Engine* engine_ = nullptr;
  SDL_Window* window_ = nullptr;
  VkExtent2D extent_;

public:
  Status init();
  WindowManager(Engine* engine, u32 width, u32 height);

  Status destroy();
  ~WindowManager();

  core::Result<char const* const*, Status> get_required_extensions(u32& count);
  core::Result<VkSurfaceKHR, Status> build_surface(VkInstance& instance);
};
} // namespace RenderEngine
