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
  auto imguiRendererStatus = state.imguiRenderer.destroy();
  auto meshRendererStatus = state.meshRenderer.destroy();
  auto bgRendererStatus = state.bgRenderer.destroy();
  auto swapchainStatus = state.swapchain.destroy();
  auto vkMgrStatus = state.vkMgr.destroy();
  auto windowMgrStatus = state.windowMgr.destroy();
  bool fail = false;
  if (imguiRendererStatus != core::code::SUCCESS) {
    ERR("failed to destroy imgui renderer");
    fail = true;
  }
  if (meshRendererStatus != core::code::SUCCESS) {
    ERR("failed to destroy asset renderer");
    fail = true;
  }
  if (bgRendererStatus != core::code::SUCCESS) {
    ERR("failed to destroy background renderer");
    fail = true;
  }
  if (swapchainStatus != core::code::SUCCESS) {
    ERR("failed to destroy swapchain");
    fail = true;
  }
  if (vkMgrStatus != core::code::SUCCESS) {
    ERR("failed to destroy vulkan manager");
    fail = true;
  }
  if (windowMgrStatus != core::code::SUCCESS) {
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
  auto& swapchain = state.swapchain;
  // we assume we are init
  swapchain.begin_commands();
  // draw
  swapchain.draw_img_transition(VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_GENERAL);
  state.bgRenderer.draw();
  swapchain.draw_img_transition(VK_IMAGE_LAYOUT_GENERAL,
                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  swapchain.depth_img_transition(VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
  state.meshRenderer.draw(camera);
  swapchain.draw_img_transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  swapchain.current_img_transition(VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  swapchain.copy_draw_to_current();
  state.imguiRenderer.draw();
  swapchain.current_img_transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  swapchain.end_commands();
}
