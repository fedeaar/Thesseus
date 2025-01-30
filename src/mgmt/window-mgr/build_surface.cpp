#include "window-manager.h"

core::Result<VkSurfaceKHR, ResourceManagement::Status>
ResourceManagement::WindowManager::build_surface(VkInstance& instance)
{
  if (!initialized) {
    logger_.error("build_surface called before initialization");
    return ResourceManagement::Status::NOT_INIT;
  }
  // create surface
  VkSurfaceKHR surface;
  if (!SDL_Vulkan_CreateSurface(window_, instance, nullptr, &surface)) {
    logger_.error(fmt::format("build_surface failed to create surface, {}",
                              SDL_GetError()));
    return ResourceManagement::Status::ERROR;
  }
  return surface;
}
