#include "engine.h"

//
// constructor
//

core::Status
render::Engine::init()
{
  if (initialized == Status::INIT) {
    return core::Status::SUCCESS;
  }
  if (initialized == Status::ERROR) {
    return core::Status::ERROR;
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
  auto status = background_renderer_.init(swapchain_);
  if (status != core::Status::SUCCESS) {
    logger_.err("init failed, swap renderer could not be created");
    return core::Status::ERROR;
  }
  status = asset_renderer_.init(swapchain_);
  if (status != core::Status::SUCCESS) {
    logger_.err("init failed, triangle mesh renderer could not be created");
    return core::Status::ERROR;
  }
  status = imgui_renderer_.init(swapchain_);
  if (status != core::Status::SUCCESS) {
    logger_.err("init failed, imgui renderer could not be created");
    return core::Status::ERROR;
  }
  initialized = Status::INIT;
  return core::Status::SUCCESS;
}

render::Engine::Engine(render::Engine::Params& params)
  : window_mgr_{ params.screen_width, params.screen_height, params.name }
  , vk_mgr_{ &window_mgr_ }
  , background_renderer_{ &vk_mgr_ }
  , asset_renderer_{ &vk_mgr_ }
  , imgui_renderer_{ &vk_mgr_, &window_mgr_ }
{
}

//
// destructor
//

core::Status
render::Engine::destroy()
{
  if (initialized == Status::NOT_INIT) {
    return core::Status::SUCCESS;
  }
  if (initialized == Status::ERROR) {
    return core::Status::ERROR;
  }
  auto imgui_renderer_status = imgui_renderer_.destroy();
  auto asset_renderer_status = asset_renderer_.destroy();
  auto background_renderer_status = background_renderer_.destroy();
  auto vk_mgr_status = vk_mgr_.destroy();
  auto window_mgr_status = window_mgr_.destroy();
  bool fail = false;
  if (imgui_renderer_status != core::Status::SUCCESS) {
    logger_.err("failed to destroy imgui renderer");
    fail = true;
  }
  if (asset_renderer_status != core::Status::SUCCESS) {
    logger_.err("failed to destroy asset renderer");
    fail = true;
  }
  if (background_renderer_status != core::Status::SUCCESS) {
    logger_.err("failed to destroy background renderer");
    fail = true;
  }
  if (vk_mgr_status != core::Status::SUCCESS) {
    logger_.err("failed to destroy vulkan manager");
    fail = true;
  }
  if (window_mgr_status != core::Status::SUCCESS) {
    logger_.err("failed to destroy window manager");
    fail = true;
  }
  if (fail) {
    initialized = Status::ERROR;
    return core::Status::ERROR;
  }
  initialized = Status::NOT_INIT;
  return core::Status::SUCCESS;
}

render::Engine::~Engine()
{
  if (initialized != Status::NOT_INIT) {
    auto status = destroy();
    if (status != core::Status::SUCCESS) {
      logger_.err("failed to destroy engine, aborting");
      abort();
    }
  }
}

//
// render
//

core::Status
render::Engine::render(Camera& camera)
{
  // we assume we are init
  auto command_buffer_result = vk_mgr_.swapchain_begin_commands(swapchain_);
  if (!command_buffer_result.has_value()) {
    return command_buffer_result.error(); // todo@engine: error msg
  }
  auto cmd = command_buffer_result.value();
  // draw
  mgmt::vulkan::image::transition_image(cmd,
                                        swapchain_.draw_img.image,
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_GENERAL);
  background_renderer_.draw(cmd, swapchain_);
  mgmt::vulkan::image::transition_image(
    cmd,
    swapchain_.draw_img.image,
    VK_IMAGE_LAYOUT_GENERAL,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  mgmt::vulkan::image::transition_image(
    cmd,
    swapchain_.depth_img.image,
    VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
  asset_renderer_.draw(cmd, swapchain_, camera);
  mgmt::vulkan::image::transition_image(
    cmd,
    swapchain_.draw_img.image,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  mgmt::vulkan::image::transition_image(cmd,
                                        swapchain_.get_current_image(),
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  mgmt::vulkan::image::copy_image(cmd,
                                  swapchain_.draw_img.image,
                                  swapchain_.get_current_image(),
                                  swapchain_.draw_extent,
                                  swapchain_.extent);
  imgui_renderer_.draw(cmd, swapchain_);
  mgmt::vulkan::image::transition_image(
    cmd,
    swapchain_.get_current_image(),
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  auto status = vk_mgr_.swapchain_end_commands(cmd, swapchain_);
  if (status != core::Status::SUCCESS) {
    return status; // todo@engine: log error message
  }
}

//
// get
//

f32
render::Engine::get_aspect_ratio()
{
  return window_mgr_.aspect_ratio;
};

f32&
render::Engine::get_render_scale()
{
  return swapchain_.render_scale;
}

mgmt::WindowManager*
render::Engine::get_window_mgr()
{
  return &window_mgr_;
};

//
// other
//

void
render::Engine::maybe_resize_swapchain()
{
  if (vk_mgr_.resize_requested) { // move
    vk_mgr_.resize_swapchain(swapchain_);
  }
}
