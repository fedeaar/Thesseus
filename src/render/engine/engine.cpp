#include "engine.h"

//
// constructor
//

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
  status = triangle_mesh_renderer_.init(swapchain_);
  if (status != core::Status::SUCCESS) {
    logger_.err("init failed, triangle mesh renderer could not be created");
    return core::Status::ERROR;
  }
  status = custom_mesh_renderer_.init(swapchain_);
  if (status != core::Status::SUCCESS) {
    logger_.err("init failed, triangle mesh renderer could not be created");
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
  , triangle_mesh_renderer_{ &vk_mgr_ }
  , custom_mesh_renderer_{ &vk_mgr_ }
  , imgui_renderer_{ &vk_mgr_, &window_mgr_ }
{
}

//
// destructor
//

core::Status
render::Engine::destroy()
{
  if (!initialized) {
    return core::Status::SUCCESS;
  }
  imgui_renderer_.destroy();
  custom_mesh_renderer_.destroy();
  triangle_mesh_renderer_.destroy();
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

//
// render
//

core::Status
render::Engine::render()
{
  // we assume we are init
  u32 img_idx;
  // await and reset command buffer
  auto command_buffer_result =
    vk_mgr_.swapchain_begin_commands(frame, swapchain_, img_idx);
  if (!command_buffer_result.has_value()) {
    return core::Status::ERROR; // todo@engine: error msg
  }
  auto cmd = command_buffer_result.value();
  // todo@engine: fixme
  swapchain_.draw_extent.width = swapchain_.draw_img.extent.width;
  swapchain_.draw_extent.height = swapchain_.draw_img.extent.height;
  // draw
  mgmt::vulkan::image::transition_image(cmd,
                                        swapchain_.draw_img.image,
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_GENERAL);
  swap_renderer_.draw(cmd, img_idx, swapchain_);
  mgmt::vulkan::image::transition_image(
    cmd,
    swapchain_.draw_img.image,
    VK_IMAGE_LAYOUT_GENERAL,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  triangle_renderer_.draw(cmd, img_idx, swapchain_);
  triangle_mesh_renderer_.draw(cmd, img_idx, swapchain_);
  custom_mesh_renderer_.draw(cmd, img_idx, swapchain_);
  mgmt::vulkan::image::transition_image(
    cmd,
    swapchain_.draw_img.image,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  mgmt::vulkan::image::transition_image(cmd,
                                        swapchain_.imgs[img_idx],
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  mgmt::vulkan::image::copy_image(cmd,
                                  swapchain_.draw_img.image,
                                  swapchain_.imgs[img_idx],
                                  swapchain_.draw_extent,
                                  swapchain_.extent);
  imgui_renderer_.draw(cmd, img_idx, swapchain_);
  mgmt::vulkan::image::transition_image(
    cmd,
    swapchain_.imgs[img_idx],
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  auto status = vk_mgr_.swapchain_end_commands(cmd, frame, img_idx, swapchain_);
  if (status != core::Status::SUCCESS) {
    return core::Status::ERROR; // todo@engine: log error message
  }
  frame++;
}
