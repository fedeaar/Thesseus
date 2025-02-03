#include "swapchain.h"

mgmt::vulkan::Swapchain::Frame const&
mgmt::vulkan::Swapchain::Swapchain::get_frame(u32 frame_number)
{
  return frames[frame_number % FRAME_OVERLAP];
}
