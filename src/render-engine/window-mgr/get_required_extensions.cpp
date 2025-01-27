#include "window-manager.h"

using namespace RenderEngine;

core::Result<char const* const*, Status>
WindowManager::get_required_extensions(u32& count)
{
  if (!initialized) {
    logger_.error("get_required_extensions called before initialization");
    return Status::ERROR;
  }
  return SDL_Vulkan_GetInstanceExtensions(&count);
}
