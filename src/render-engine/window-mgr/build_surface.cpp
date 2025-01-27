#include "window-manager.h"

using namespace RenderEngine;

core::Result<VkSurfaceKHR, Status>
WindowManager::build_surface(VkInstance& instance)
{
  if (!initialized) {
    logger_.error("build_surface called before initialization");
    return Status::ERROR;
  }
  // create surface
  VkSurfaceKHR surface;
  SDL_Vulkan_CreateSurface(window_, instance, nullptr, &surface);
  return surface;
}
