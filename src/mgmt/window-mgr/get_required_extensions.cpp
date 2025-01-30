#include "window-manager.h"

core::Result<char const* const*, ResourceManagement::Status>
ResourceManagement::WindowManager::get_required_extensions(u32& count)
{
  if (!initialized) {
    logger_.error("get_required_extensions called before initialization");
    return ResourceManagement::Status::NOT_INIT;
  }
  auto extensions = SDL_Vulkan_GetInstanceExtensions(&count);
  if (extensions == NULL) {
    logger_.error(
      fmt::format("get_required_extensions failed, error: {}", SDL_GetError()));
    return ResourceManagement::Status::ERROR;
  }
  return extensions;
}
