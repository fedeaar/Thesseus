#include "window-manager.h"

ResourceManagement::Status
ResourceManagement::WindowManager::destroy()
{
  if (!initialized) {
    logger_.error("destroy called before initialization");
    return ResourceManagement::Status::SUCCESS;
  }
  // destroy window
  SDL_DestroyWindow(window_);
  window_ = nullptr;
  // success
  initialized = false;
  return ResourceManagement::Status::SUCCESS;
}

ResourceManagement::WindowManager::~WindowManager()
{
  if (initialized) {
    destroy();
  }
};
