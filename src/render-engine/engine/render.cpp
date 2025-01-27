#include "engine.h"

using namespace RenderEngine;

RenderEngine::Status
Engine::render()
{
  auto status = swapchain_.await_render();
}
