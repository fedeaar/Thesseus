#include "window-manager.h"

ResourceManagement::Status
ResourceManagement::WindowManager::init()
{
  if (initialized) {
    return ResourceManagement::Status::SUCCESS;
  }
  // initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    logger_.error(
      fmt::format("init failed to initialize SDL, {}", SDL_GetError()));
    return ResourceManagement::Status::ERROR;
  }
  // create window
  u32 flags = (u32)(SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED);
  window_ =
    SDL_CreateWindow(window_name.c_str(), extent_.width, extent_.height, flags);
  if (window_ == nullptr) {
    logger_.error(
      fmt::format("init failed to create window, {}", SDL_GetError()));
    return ResourceManagement::Status::ERROR;
  }
  // success
  initialized = true;
  return ResourceManagement::Status::SUCCESS;
}

ResourceManagement::WindowManager::WindowManager(u32 width,
                                                 u32 height,
                                                 std::string const& window_name)
  : width{ width }
  , height{ height }
  , extent_{ width, height }
  , aspect_ratio{ (f32)width / (f32)height }
  , window_name{ window_name } {};
