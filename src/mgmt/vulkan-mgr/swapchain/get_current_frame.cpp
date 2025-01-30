#include "swapchain.h"

ResourceManagement::VulkanManager::Swapchain::Frame const&
ResourceManagement::VulkanManager::Swapchain::Swapchain::get_frame(
  u32 frame_number)
{
  return frames[frame_number % FRAME_OVERLAP];
}
