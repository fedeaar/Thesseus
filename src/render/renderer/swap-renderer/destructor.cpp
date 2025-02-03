#include "swap-renderer.h"

core::Status
render::SwapRenderer::destroy()
{
  // todo@engine: handle pipes?
  initialized = false;
  return core::Status::SUCCESS;
}

render::SwapRenderer::~SwapRenderer()
{
  if (initialized) {
    destroy();
  }
}
