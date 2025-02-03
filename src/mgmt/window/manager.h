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
  std::string const namespace_ = mgmt::namespace_ + "::WindowManager";
  core::Logger logger_{ namespace_ };

  SDL_Window* window_ = nullptr;
  VkExtent2D extent_;

public:
  WindowManager(u32 width, u32 height, std::string const& window_name);
  ~WindowManager();

  core::Status init();
  core::Status destroy();

  SDL_Window* get_window();
  VkExtent2D& get_extent();

  core::Result<char const* const*, core::Status> get_required_extensions(
    u32& count);
  core::Status build_surface(VkInstance& instance, VkSurfaceKHR* surface);
};
} // namespace mgmt
