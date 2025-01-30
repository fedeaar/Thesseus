#include "engine.h"

RenderEngine::Status
RenderEngine::Engine::destroy()
{
  if (!initialized) {
    return RenderEngine::Status::SUCCESS;
  }
  auto status = vk_mgr_.destroy();
  if (status != ResourceManagement::Status::SUCCESS) {
    logger_.error("failed to destroy vulkan manager");
    return RenderEngine::Status::ERROR;
  }
  status = window_mgr_.destroy();
  if (status != ResourceManagement::Status::SUCCESS) {
    logger_.error("failed to destroy window manager");
    return RenderEngine::Status::ERROR;
  }
  swap_renderer_.destroy();
  imgui_renderer_.destroy();
  return RenderEngine::Status::SUCCESS;
}

RenderEngine::Engine::~Engine()
{
  if (initialized) {
    auto status = destroy();
    if (status != RenderEngine::Status::SUCCESS) {
      logger_.error("failed to destroy engine, aborting");
      abort();
    }
  }
}
