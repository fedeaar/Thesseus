#include "engine.h"

//
// constructor
//

core::code
render::Engine::init()
{
  if (initialized == core::status::INIT) {
    return core::code::SUCCESS;
  }
  if (initialized == core::status::ERROR) {
    return core::code::ERROR;
  }
  if (window_mgr_.init() != core::code::SUCCESS) {
    core::Logger::err("render::Engine::init",
                      "window mgr could not be created");
    return core::code::ERROR;
  }
  if (vk_mgr_.init() != core::code::SUCCESS) {
    core::Logger::err("render::Engine::init", "vk mgr could not be created");
    return core::code::ERROR;
  }
  auto swapchain_result = vk_mgr_.create_swapchain();
  if (!swapchain_result.has_value()) {
    core::Logger::err("render::Engine::init", "swapchain could not be created");
    return core::code::ERROR;
  }
  swapchain_ = swapchain_result.value();
  auto status = background_renderer_.init(swapchain_);
  if (status != core::code::SUCCESS) {
    core::Logger::err("render::Engine::init",
                      "swap renderer could not be created");
    return core::code::ERROR;
  }
  status = asset_renderer_.init(swapchain_);
  if (status != core::code::SUCCESS) {
    core::Logger::err("render::Engine::init",
                      "triangle mesh renderer could not be created");
    return core::code::ERROR;
  }
  status = imgui_renderer_.init(swapchain_);
  if (status != core::code::SUCCESS) {
    core::Logger::err("render::Engine::init",
                      "imgui renderer could not be created");
    return core::code::ERROR;
  }
  initialized = core::status::INIT;
  return core::code::SUCCESS;
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

core::code
render::Engine::destroy()
{
  if (initialized == core::status::NOT_INIT) {
    return core::code::SUCCESS;
  }
  if (initialized == core::status::ERROR) {
    return core::code::ERROR;
  }
  auto imgui_renderer_status = imgui_renderer_.destroy();
  auto asset_renderer_status = asset_renderer_.destroy();
  auto background_renderer_status = background_renderer_.destroy();
  auto vk_mgr_status = vk_mgr_.destroy();
  auto window_mgr_status = window_mgr_.destroy();
  bool fail = false;
  if (imgui_renderer_status != core::code::SUCCESS) {
    core::Logger::err("render::Engine::destroy",
                      "failed to destroy imgui renderer");
    fail = true;
  }
  if (asset_renderer_status != core::code::SUCCESS) {
    core::Logger::err("render::Engine::destroy",
                      "failed to destroy asset renderer");
    fail = true;
  }
  if (background_renderer_status != core::code::SUCCESS) {
    core::Logger::err("render::Engine::destroy",
                      "failed to destroy background renderer");
    fail = true;
  }
  if (vk_mgr_status != core::code::SUCCESS) {
    core::Logger::err("render::Engine::destroy",
                      "failed to destroy vulkan manager");
    fail = true;
  }
  if (window_mgr_status != core::code::SUCCESS) {
    core::Logger::err("render::Engine::destroy",
                      "failed to destroy window manager");
    fail = true;
  }
  if (fail) {
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  initialized = core::status::NOT_INIT;
  return core::code::SUCCESS;
}

render::Engine::~Engine()
{
  if (initialized != core::status::NOT_INIT) {
    auto status = destroy();
    if (status != core::code::SUCCESS) {
      core::Logger::err("render::Engine::~Engine",
                        "failed to destroy engine, aborting");
      abort();
    }
  }
}

//
// render
//

void
render::Engine::render(Camera& camera)
{
  // we assume we are init
  vk_mgr_.swapchain_begin_commands(swapchain_);
  // draw
  swapchain_.draw_img_transition(VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_GENERAL);
  background_renderer_.draw(swapchain_);
  swapchain_.draw_img_transition(VK_IMAGE_LAYOUT_GENERAL,
                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  swapchain_.depth_img_transition(VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
  asset_renderer_.draw(swapchain_, camera);
  swapchain_.draw_img_transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  swapchain_.current_img_transition(VK_IMAGE_LAYOUT_UNDEFINED,
                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  mgmt::vulkan::image::copy_image(swapchain_.get_current_cmd_buffer(),
                                  swapchain_.draw_img.image,
                                  swapchain_.get_current_image(),
                                  swapchain_.draw_extent,
                                  swapchain_.extent);
  imgui_renderer_.draw(swapchain_);
  swapchain_.current_img_transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  vk_mgr_.swapchain_end_commands(swapchain_);
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
