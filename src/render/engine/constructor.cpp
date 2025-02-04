#include "../renderer/imgui-renderer/imgui-renderer.h"
#include "../renderer/swap-renderer/swap-renderer.h"
#include "engine.h"

core::Status
render::Engine::init()
{
  if (initialized) {
    return core::Status::SUCCESS;
  }
  if (window_mgr_.init() != core::Status::SUCCESS) {
    logger_.err("init failed, window mgr could not be created");
    return core::Status::ERROR;
  }
  if (vk_mgr_.init() != core::Status::SUCCESS) {
    logger_.err("init failed, vk mgr could not be created");
    return core::Status::ERROR;
  }
  auto swapchain_result = vk_mgr_.create_swapchain();
  if (!swapchain_result.has_value()) {
    logger_.err("init failed, swapchain could not be created");
    return core::Status::ERROR;
  }
  swapchain_ = swapchain_result.value();
  auto status = swap_renderer_.init(swapchain_);
  if (status != core::Status::SUCCESS) {
    logger_.err("init failed, swap renderer could not be created");
    return core::Status::ERROR;
  }
  status = triangle_renderer_.init(swapchain_);
  if (status != core::Status::SUCCESS) {
    logger_.err("init failed, triangle renderer could not be created");
    return core::Status::ERROR;
  }
  status = imgui_renderer_.init(swapchain_);
  if (status != core::Status::SUCCESS) {
    logger_.err("init failed, imgui renderer could not be created");
    return core::Status::ERROR;
  }
  initialized = true;
  return core::Status::SUCCESS;
}

render::Engine::Engine(render::Engine::Params& params)
  : params_{ params }
  , window_mgr_{ params.screen_width, params.screen_height, params.name }
  , vk_mgr_{ &window_mgr_ }
  , swap_renderer_{ &vk_mgr_ }
  , triangle_renderer_{ &vk_mgr_ }
  , imgui_renderer_{ &vk_mgr_, &window_mgr_ }
{
}
