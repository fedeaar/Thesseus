#include "engine.h"

core::Status
render::Engine::destroy()
{
  if (!initialized) {
    return core::Status::SUCCESS;
  }
  imgui_renderer_.destroy();
  triangle_renderer_.destroy();
  swap_renderer_.destroy();
  auto status = vk_mgr_.destroy();
  if (status != core::Status::SUCCESS) {
    logger_.err("failed to destroy vulkan manager");
    return core::Status::ERROR;
  }
  status = window_mgr_.destroy();
  if (status != core::Status::SUCCESS) {
    logger_.err("failed to destroy window manager");
    return core::Status::ERROR;
  }
  initialized = false;
  return core::Status::SUCCESS;
}

render::Engine::~Engine()
{
  if (initialized) {
    auto status = destroy();
    if (status != core::Status::SUCCESS) {
      logger_.err("failed to destroy engine, aborting");
      abort();
    }
  }
}
