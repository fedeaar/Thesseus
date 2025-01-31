#pragma once

#include "../mgmt.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

namespace ResourceManagement {

class WindowManager
{
public:
  bool initialized = false;
  u32 width;
  u32 height;
  f32 aspect_ratio;
  std::string const& window_name;

private:
  std::string const namespace_ =
    ResourceManagement::namespace_ + "::WindowManager";
  core::Logger logger_{ namespace_ };

  SDL_Window* window_ = nullptr;
  VkExtent2D extent_;

public:
  Status init();
  WindowManager(u32 width, u32 height, std::string const& window_name);

  Status destroy();
  ~WindowManager();

  SDL_Window* get_window();

  core::Result<char const* const*, Status> get_required_extensions(u32& count);
  Status build_surface(VkInstance& instance, VkSurfaceKHR& surface);
};
} // namespace ResourceManagement
