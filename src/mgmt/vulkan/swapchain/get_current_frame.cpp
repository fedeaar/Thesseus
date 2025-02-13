#include "swapchain.h"

mgmt::vulkan::swapchain::Frame&
mgmt::vulkan::swapchain::Swapchain::get_current_frame()
{
  return frames[frame % FRAME_OVERLAP];
}

VkImage&
mgmt::vulkan::swapchain::Swapchain::get_current_image()
{
  return imgs[current_img_idx];
}

VkImageView&
mgmt::vulkan::swapchain::Swapchain::get_current_image_view()
{
  return img_views[current_img_idx];
}
