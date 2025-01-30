#include "swap-renderer.h"

RenderEngine::Status
RenderEngine::SwapRenderer::destroy()
{
  // todo@engine: handle pipes?
  return RenderEngine::Status::SUCCESS;
}

RenderEngine::SwapRenderer::~SwapRenderer()
{
  destroy();
}
