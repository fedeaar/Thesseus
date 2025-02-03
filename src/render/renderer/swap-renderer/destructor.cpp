#include "swap-renderer.h"

core::Status
render::SwapRenderer::destroy()
{
  // todo@engine: handle pipes?
  return core::Status::SUCCESS;
}

render::SwapRenderer::~SwapRenderer()
{
  destroy();
}
