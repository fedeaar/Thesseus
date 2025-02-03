#include "imgui-renderer.h"

core::Status
render::ImguiRenderer::destroy()
{
  if (!initialized) {
    return core::Status::SUCCESS;
  }
  vkDeviceWaitIdle(vk_mgr_->get_dev());
  ImGui_ImplVulkan_Shutdown();
  initialized = false;
  return core::Status::SUCCESS;
}

render::ImguiRenderer::~ImguiRenderer()
{
  destroy();
}
