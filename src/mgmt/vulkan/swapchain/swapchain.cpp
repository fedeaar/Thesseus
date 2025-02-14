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

VkCommandBuffer&
mgmt::vulkan::swapchain::Swapchain::get_current_cmd_buffer()
{
  return get_current_frame().main_command_buffer;
}

core::code
mgmt::vulkan::swapchain::Swapchain::draw_img_transition(VkImageLayout current,
                                                        VkImageLayout next)
{
  return image::transition_image(
    get_current_cmd_buffer(), draw_img.image, current, next);
}

core::code
mgmt::vulkan::swapchain::Swapchain::depth_img_transition(VkImageLayout current,
                                                         VkImageLayout next)
{
  return image::transition_image(
    get_current_cmd_buffer(), depth_img.image, current, next);
}

core::code
mgmt::vulkan::swapchain::Swapchain::current_img_transition(
  VkImageLayout current,
  VkImageLayout next)
{
  return image::transition_image(
    get_current_cmd_buffer(), get_current_image(), current, next);
}
