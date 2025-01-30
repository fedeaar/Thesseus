#include "imgui-renderer.h"

RenderEngine::Status
RenderEngine::ImguiRenderer::destroy()
{
  if (!initialized) {
    return RenderEngine::Status::SUCCESS;
  }
  ImGui_ImplVulkan_Shutdown();
  initialized = false;
  return RenderEngine::Status::SUCCESS;
}

RenderEngine::ImguiRenderer::~ImguiRenderer()
{
  destroy();
}
