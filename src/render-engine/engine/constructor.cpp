#include "../renderer/imgui-renderer/imgui-renderer.h"
#include "../renderer/swap-renderer/swap-renderer.h"
#include "engine.h"

RenderEngine::Status
RenderEngine::Engine::init()
{
  if (initialized) {
    return RenderEngine::Status::SUCCESS;

    if (window_mgr_.init() != ResourceManagement::Status::SUCCESS) {
      logger_.error("init failed, window mgr could not be created");
      return RenderEngine::Status::ERROR;
    }
    if (vk_mgr_.init() != ResourceManagement::Status::SUCCESS) {
      logger_.error("init failed, vk mgr could not be created");
      return RenderEngine::Status::ERROR;
    }
    auto swapchain_result = vk_mgr_.create_swapchain();
    if (!swapchain_result.has_value()) {
      logger_.error("init failed, swapchain could not be created");
      return RenderEngine::Status::ERROR;
    }
    swapchain_ = swapchain_result.value();
    auto status = swap_renderer_.init(swapchain_);
    if (status != ResourceManagement::Status::SUCCESS) {
      logger_.error("init failed, swap renderer could not be created");
      return RenderEngine::Status::ERROR; // todo@engine: log error
    }
    status = imgui_renderer_.init(swapchain_);
    if (status != ResourceManagement::Status::SUCCESS) {
      logger_.error("init failed, imgui renderer could not be created");
      return RenderEngine::Status::ERROR; // todo@engine: log error
    }
    initialized = true;
    return RenderEngine::Status::SUCCESS;
  }
}

RenderEngine::Engine::Engine(RenderEngine::Engine::Params& params)
  : params_{ params }
  , window_mgr_{ params.screen_width, params.screen_height, params.name }
  , vk_mgr_{ &window_mgr_ }
  , swap_renderer_{ &vk_mgr_ }
  , imgui_renderer_{ &vk_mgr_, &window_mgr_ }
{
}
