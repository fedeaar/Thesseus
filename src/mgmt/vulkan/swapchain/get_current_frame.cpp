#include "swapchain.h"

mgmt::vulkan::swapchain::Frame const&
mgmt::vulkan::swapchain::Swapchain::get_frame(u32 frame_number)
{
  return frames[frame_number % FRAME_OVERLAP];
}
