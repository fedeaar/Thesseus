#include "window-manager.h"

ResourceManagement::Status
ResourceManagement::WindowManager::build_surface(VkInstance& instance,
                                                 VkSurfaceKHR& surface)
{
  if (!initialized) {
    logger_.error("build_surface called before initialization");
    return ResourceManagement::Status::NOT_INIT;
  }
  // create surface
  if (!SDL_Vulkan_CreateSurface(window_, instance, nullptr, &surface)) {
    logger_.error(fmt::format("build_surface failed to create surface, {}",
                              SDL_GetError()));
    return ResourceManagement::Status::ERROR;
  }
  return ResourceManagement::Status::SUCCESS;
}
