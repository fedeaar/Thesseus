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
  if (state.windowMgr.init() != core::code::SUCCESS) {
    ERR("window mgr could not be created");
    return core::code::ERROR;
  }
  if (state.vkMgr.init() != core::code::SUCCESS) {
    ERR("vk mgr could not be created");
    return core::code::ERROR;
  }
  if (state.swapchain.init() != core::code::SUCCESS) {
    ERR("swapchain could not be created");
    return core::code::ERROR;
  }
  if (state.bgRenderer.init() != core::code::SUCCESS) {
    ERR("bg renderer could not be created");
    return core::code::ERROR;
  }
  if (state.meshRenderer.init() != core::code::SUCCESS) {
    ERR("mesh renderer could not be created");
    return core::code::ERROR;
  }
  if (state.imguiRenderer.init() != core::code::SUCCESS) {
    ERR("imgui renderer could not be created");
    return core::code::ERROR;
  }
  state.initialized = core::status::INITIALIZED;
  return core::code::SUCCESS;
}

render::Engine::Engine(render::Engine::Params& params)
  : state{ .initialized = core::status::NOT_INITIALIZED,
           .windowMgr{ params.screenWidth, params.screenHeight, params.name },
           .vkMgr{ &state.windowMgr },
           .swapchain{ &state.vkMgr },
           .bgRenderer{ &state.swapchain, &state.vkMgr },
           .meshRenderer{ &state.swapchain, &state.vkMgr },
           .imguiRenderer{ &state.swapchain, &state.vkMgr, &state.windowMgr } }
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
  auto imgui_renderer_status = state.imguiRenderer.destroy();
  auto mesh_renderer_status = state.meshRenderer.destroy();
  auto bg_renderer_status = state.bgRenderer.destroy();
  auto swapchain_status = state.swapchain.destroy();
  auto vk_mgr_status = state.vkMgr.destroy();
  auto window_mgr_status = state.windowMgr.destroy();
  bool fail = false;
  if (imgui_renderer_status != core::code::SUCCESS) {
    ERR("failed to destroy imgui renderer");
    fail = true;
  }
  if (mesh_renderer_status != core::code::SUCCESS) {
    ERR("failed to destroy asset renderer");
    fail = true;
  }
  if (bg_renderer_status != core::code::SUCCESS) {
    ERR("failed to destroy background renderer");
    fail = true;
  }
  if (swapchain_status != core::code::SUCCESS) {
    ERR("failed to destroy swapchain");
    fail = true;
  }
  if (vk_mgr_status != core::code::SUCCESS) {
    ERR("failed to destroy vulkan manager");
    fail = true;
  }
  if (window_mgr_status != core::code::SUCCESS) {
    ERR("failed to destroy window manager");
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
      ERR("failed to destroy engine, aborting");
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
  state.bgRenderer.draw();
  state.swapchain.draw_img_transition(VK_IMAGE_LAYOUT_GENERAL,
                                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  state.swapchain.depth_img_transition(
    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
  state.meshRenderer.update_scene(camera);
  state.meshRenderer.draw(camera);
  state.swapchain.draw_img_transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  state.swapchain.current_img_transition(VK_IMAGE_LAYOUT_UNDEFINED,
                                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  state.swapchain.copy_draw_to_current();
  state.imguiRenderer.draw();
  state.swapchain.current_img_transition(
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  state.swapchain.end_commands();
}
