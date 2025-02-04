#include "manager.h"

core::Status
mgmt::vulkan::Manager::swapchain_end_commands(VkCommandBuffer cmd,
                                              u32 frame_number,
                                              u32 img_idx,
                                              Swapchain::Swapchain& swapchain)
{
  auto status = check(vkEndCommandBuffer(cmd));
  if (status != core::Status::SUCCESS) {
    return core::Status::ERROR; // todo@mgmt: log error msg
  }
  auto current_frame = swapchain.get_frame(frame_number);
  // prepare the submission to the queue
  VkCommandBufferSubmitInfo cmd_submit_info =
    info::command_buffer_submit_info(cmd);
  VkSemaphoreSubmitInfo wait_info = mgmt::vulkan::info::semaphore_submit_info(
    VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
    current_frame.swapchain_semaphore);
  VkSemaphoreSubmitInfo signal_info = mgmt::vulkan::info::semaphore_submit_info(
    VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, current_frame.render_semaphore);
  VkSubmitInfo2 submit =
    mgmt::vulkan::info::submit_info(&cmd_submit_info, &signal_info, &wait_info);
  // submit
  status = check(
    vkQueueSubmit2(graphics_queue_, 1, &submit, current_frame.render_fence));
  if (status != core::Status::SUCCESS) {
    return core::Status::ERROR; // todo@mgmt: log error msg
  }
  // display
  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.pNext = nullptr;
  present_info.pSwapchains = &swapchain.swapchain;
  present_info.swapchainCount = 1;
  present_info.pWaitSemaphores = &current_frame.render_semaphore;
  present_info.waitSemaphoreCount = 1;
  present_info.pImageIndices = &img_idx;
  status = check(vkQueuePresentKHR(graphics_queue_, &present_info));
  if (status != core::Status::SUCCESS) {
    return core::Status::ERROR; // todo@mgmt: log error msg
  }
  // success
  return core::Status::SUCCESS;
}
