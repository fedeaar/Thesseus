#pragma once

#include "../mgmt.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

namespace mgmt {

namespace window {

class Manager
{
public:
  struct State
  {
    core::status status = core::status::NOT_INIT;
    VkExtent2D extent;
    f32 aspect_ratio;

    std::string window_name;
  } state;

private:
  SDL_Window* window_ = nullptr;

public:
  core::code init();
  Manager(u32 width, u32 height, std::string const& window_name);

  core::code destroy();
  ~Manager();

  SDL_Window* get_window();
  State& get_state();
  core::Result<char const* const*, core::code> get_required_extensions(
    u32& count);

  core::code build_surface(VkInstance& instance, VkSurfaceKHR* surface);
};

} // namespace window
} // namespace mgmt
