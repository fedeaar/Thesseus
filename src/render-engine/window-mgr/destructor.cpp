#include "window-manager.h"

using namespace RenderEngine;

Status
WindowManager::destroy()
{
  if (!initialized) {
    logger_.log("destroy called before initialization");
    return Status::SUCCESS;
  }
  // destroy window
  SDL_DestroyWindow(window_);
  window_ = nullptr;
  // success
  initialized = false;
  return Status::SUCCESS;
}

WindowManager::~WindowManager()
{
  if (initialized) {
    destroy();
  }
};
