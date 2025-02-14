#pragma once

#include "../mgmt.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

namespace mgmt {

class WindowManager
{
public:
  bool initialized = false;
  u32 width;
  u32 height;
  f32 aspect_ratio;
  std::string const& window_name;

private:
  SDL_Window* window_ = nullptr;
  VkExtent2D extent_;

public:
  WindowManager(u32 width, u32 height, std::string const& window_name);
  ~WindowManager();

  core::code init();
  core::code destroy();

  SDL_Window* get_window();
  VkExtent2D& get_extent();

  core::Result<char const* const*, core::code> get_required_extensions(
    u32& count);
  core::code build_surface(VkInstance& instance, VkSurfaceKHR* surface);
};
} // namespace mgmt
