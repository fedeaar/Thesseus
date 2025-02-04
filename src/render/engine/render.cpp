#include "engine.h"

core::Status
render::Engine::render()
{
  // we assume we are init
  u32 img_idx;
  // await and reset command buffer
  auto command_buffer_result =
    vk_mgr_.swapchain_begin_commands(frame, swapchain_, img_idx);
  if (!command_buffer_result.has_value()) {
    return core::Status::ERROR; // todo@engine: error msg
  }
  auto cmd = command_buffer_result.value();
  // todo@engine: fixme
  swapchain_.draw_extent.width = swapchain_.draw_img.extent.width;
  swapchain_.draw_extent.height = swapchain_.draw_img.extent.height;
  // draw
  mgmt::vulkan::image::transition_image(cmd,
                                        swapchain_.draw_img.image,
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_GENERAL);
  swap_renderer_.draw(cmd, img_idx, swapchain_);
  mgmt::vulkan::image::transition_image(
    cmd,
    swapchain_.draw_img.image,
    VK_IMAGE_LAYOUT_GENERAL,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  triangle_renderer_.draw(cmd, img_idx, swapchain_);
  mgmt::vulkan::image::transition_image(
    cmd,
    swapchain_.draw_img.image,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  mgmt::vulkan::image::transition_image(cmd,
                                        swapchain_.imgs[img_idx],
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  mgmt::vulkan::image::copy_image(cmd,
                                  swapchain_.draw_img.image,
                                  swapchain_.imgs[img_idx],
                                  swapchain_.draw_extent,
                                  swapchain_.extent);
  imgui_renderer_.draw(cmd, img_idx, swapchain_);
  mgmt::vulkan::image::transition_image(
    cmd,
    swapchain_.imgs[img_idx],
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  auto status = vk_mgr_.swapchain_end_commands(cmd, frame, img_idx, swapchain_);
  if (status != core::Status::SUCCESS) {
    return core::Status::ERROR; // todo@engine: log error message
  }
  frame++;
}
