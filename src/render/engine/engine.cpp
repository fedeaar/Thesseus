#include "engine.h"

//
// constructor
//

core::code
render::Engine::init()
{
  if (state.initialized == core::status::INIT) {
    return core::code::SUCCESS;
  }
  if (state.initialized == core::status::ERROR) {
    return core::code::ERROR;
  }
  if (state.window_mgr.init() != core::code::SUCCESS) {
    core::Logger::err("render::Engine::init",
                      "window mgr could not be created");
    return core::code::ERROR;
  }
  if (state.vk_mgr.init() != core::code::SUCCESS) {
    core::Logger::err("render::Engine::init", "vk mgr could not be created");
    return core::code::ERROR;
  }
  auto swapchain_result = state.vk_mgr.create_swapchain();
  if (!swapchain_result.has_value()) {
    core::Logger::err("render::Engine::init", "swapchain could not be created");
    return core::code::ERROR;
  }
  state.swapchain = swapchain_result.value();
  auto status = background_renderer_.init(state.swapchain);
  if (status != core::code::SUCCESS) {
    core::Logger::err("render::Engine::init",
                      "swap renderer could not be created");
    return core::code::ERROR;
  }
  status = asset_renderer_.init(state.swapchain);
  if (status != core::code::SUCCESS) {
    core::Logger::err("render::Engine::init",
                      "triangle mesh renderer could not be created");
    return core::code::ERROR;
  }
  status = imgui_renderer_.init(state.swapchain);
  if (status != core::code::SUCCESS) {
    core::Logger::err("render::Engine::init",
                      "imgui renderer could not be created");
    return core::code::ERROR;
  }
  state.initialized = core::status::INIT;
  return core::code::SUCCESS;
}

render::Engine::Engine(render::Engine::Params& params)
  : state{ .window_mgr{ params.screen_width,
                        params.screen_height,
                        params.name },
           .vk_mgr{ &state.window_mgr } }
  , background_renderer_{ &state.vk_mgr }
  , asset_renderer_{ &state.vk_mgr }
  , imgui_renderer_{ &state.vk_mgr, &state.window_mgr }
{
}

//
// destructor
//

core::code
render::Engine::destroy()
{
  if (state.initialized == core::status::NOT_INIT) {
    return core::code::SUCCESS;
  }
  if (state.initialized == core::status::ERROR) {
    return core::code::ERROR;
  }
  auto imgui_renderer_status = imgui_renderer_.destroy();
  auto asset_renderer_status = asset_renderer_.destroy();
  auto background_renderer_status = background_renderer_.destroy();
  auto vk_mgr_status = state.vk_mgr.destroy();
  auto window_mgr_status = state.window_mgr.destroy();
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
    state.initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  state.initialized = core::status::NOT_INIT;
  return core::code::SUCCESS;
}

render::Engine::~Engine()
{
  if (state.initialized != core::status::NOT_INIT) {
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
  state.vk_mgr.swapchain_begin_commands(state.swapchain);
  // draw
  state.swapchain.draw_img_transition(VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_GENERAL);
  background_renderer_.draw(state.swapchain);
  state.swapchain.draw_img_transition(VK_IMAGE_LAYOUT_GENERAL,
                                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  state.swapchain.depth_img_transition(
    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
  asset_renderer_.draw(state.swapchain, camera);
  state.swapchain.draw_img_transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  state.swapchain.current_img_transition(VK_IMAGE_LAYOUT_UNDEFINED,
                                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  mgmt::vulkan::image::copy_image(state.swapchain.get_current_cmd_buffer(),
                                  state.swapchain.draw_img.image,
                                  state.swapchain.get_current_image(),
                                  state.swapchain.draw_extent,
                                  state.swapchain.extent);
  imgui_renderer_.draw(state.swapchain);
  state.swapchain.current_img_transition(
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  state.vk_mgr.swapchain_end_commands(state.swapchain);
}
