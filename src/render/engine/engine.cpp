#include "engine.h"

//
// constructor
//

core::code
render::Engine::init()
{
  if (state.initialized == core::status::INITIALIZED) {
    return core::code::SUCCESS;
  }
  if (state.initialized == core::status::ERROR) {
    return core::code::ERROR;
  }
  if (state.window_mgr.init() != core::code::SUCCESS) {
    core::Logger::err(FUNCTION_NAME, "window mgr could not be created");
    return core::code::ERROR;
  }
  if (state.vk_mgr.init() != core::code::SUCCESS) {
    core::Logger::err(FUNCTION_NAME, "vk mgr could not be created");
    return core::code::ERROR;
  }
  if (state.swapchain.init() != core::code::SUCCESS) {
    core::Logger::err(FUNCTION_NAME, "swapchain could not be created");
    return core::code::ERROR;
  }
  if (state.bg_renderer.init(state.swapchain) != core::code::SUCCESS) {
    core::Logger::err(FUNCTION_NAME, "bg renderer could not be created");
    return core::code::ERROR;
  }
  if (state.mesh_renderer.init(state.swapchain) != core::code::SUCCESS) {
    core::Logger::err(FUNCTION_NAME, "mesh renderer could not be created");
    return core::code::ERROR;
  }
  if (state.imgui_renderer.init(state.swapchain) != core::code::SUCCESS) {
    core::Logger::err(FUNCTION_NAME, "imgui renderer could not be created");
    return core::code::ERROR;
  }
  state.initialized = core::status::INITIALIZED;
  return core::code::SUCCESS;
}

render::Engine::Engine(render::Engine::Params& params)
  : state{
    .initialized = core::status::NOT_INITIALIZED,
    .window_mgr{ params.screen_width, params.screen_height, params.name },
    .vk_mgr{ &state.window_mgr },
    .swapchain{ &state.vk_mgr },
    .bg_renderer{ &state.vk_mgr },
    .mesh_renderer{ &state.vk_mgr },
    .imgui_renderer{ &state.vk_mgr, &state.window_mgr }
  }
{
}

//
// destructor
//

core::code
render::Engine::destroy()
{
  if (state.initialized == core::status::NOT_INITIALIZED) {
    return core::code::SUCCESS;
  }
  if (state.initialized == core::status::ERROR) {
    return core::code::ERROR;
  }
  auto imgui_renderer_status = state.imgui_renderer.destroy();
  auto mesh_renderer_status = state.mesh_renderer.destroy();
  auto bg_renderer_status = state.bg_renderer.destroy();
  auto swapchain_status = state.swapchain.destroy();
  auto vk_mgr_status = state.vk_mgr.destroy();
  auto window_mgr_status = state.window_mgr.destroy();
  bool fail = false;
  if (imgui_renderer_status != core::code::SUCCESS) {
    core::Logger::err(FUNCTION_NAME, "failed to destroy imgui renderer");
    fail = true;
  }
  if (mesh_renderer_status != core::code::SUCCESS) {
    core::Logger::err(FUNCTION_NAME, "failed to destroy asset renderer");
    fail = true;
  }
  if (bg_renderer_status != core::code::SUCCESS) {
    core::Logger::err(FUNCTION_NAME, "failed to destroy background renderer");
    fail = true;
  }
  if (swapchain_status != core::code::SUCCESS) {
    core::Logger::err(FUNCTION_NAME, "failed to destroy swapchain");
    fail = true;
  }
  if (vk_mgr_status != core::code::SUCCESS) {
    core::Logger::err(FUNCTION_NAME, "failed to destroy vulkan manager");
    fail = true;
  }
  if (window_mgr_status != core::code::SUCCESS) {
    core::Logger::err(FUNCTION_NAME, "failed to destroy window manager");
    fail = true;
  }
  if (fail) {
    state.initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  state.initialized = core::status::NOT_INITIALIZED;
  return core::code::SUCCESS;
}

render::Engine::~Engine()
{
  if (state.initialized != core::status::NOT_INITIALIZED) {
    auto status = destroy();
    if (status != core::code::SUCCESS) {
      core::Logger::err(FUNCTION_NAME, "failed to destroy engine, aborting");
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
  state.swapchain.begin_commands();
  // draw
  state.swapchain.draw_img_transition(VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_GENERAL);
  state.bg_renderer.draw(state.swapchain);
  state.swapchain.draw_img_transition(VK_IMAGE_LAYOUT_GENERAL,
                                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  state.swapchain.depth_img_transition(
    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
  state.mesh_renderer.update_scene(state.swapchain, camera);
  state.mesh_renderer.draw(state.swapchain, camera);
  state.swapchain.draw_img_transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  state.swapchain.current_img_transition(VK_IMAGE_LAYOUT_UNDEFINED,
                                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  state.swapchain.copy_draw_to_current();
  state.imgui_renderer.draw(state.swapchain);
  state.swapchain.current_img_transition(
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  state.swapchain.end_commands();
}
