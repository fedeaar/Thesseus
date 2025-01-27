#include "swapchain.h"

using namespace RenderEngine;
using namespace RenderEngine::VulkanManager;

Swapchain::Frame const&
Swapchain::get_current_frame()
{
  return params_.frames[frame_ % SWAPCHAIN_FRAME_OVERLAP];
}
