#include "window-manager.h"

using namespace RenderEngine;

Status
WindowManager::init()
{
  if (initialized) {
    return Status::SUCCESS;
  }
  auto engine = *engine_;
  // initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    logger_.error(fmt::format("failed to initialize: {}", SDL_GetError()));
    return Status::ERROR;
  }
  // create window
  u32 flags = (u32)(SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED);
  window_ =
    SDL_CreateWindow(engine.name.c_str(), extent_.width, extent_.height, flags);
  if (window_ == nullptr) {
    logger_.error(fmt::format("failed to create window: {}", SDL_GetError()));
    return Status::ERROR;
  }
  // success
  initialized = true;
  return Status::SUCCESS;
}

WindowManager::WindowManager(Engine* engine, u32 width, u32 height)
  : engine_{ engine }
  , width{ width }
  , height{ height }
  , extent_{ width, height }
  , aspect_ratio{ (f32)width / (f32)height } {};
