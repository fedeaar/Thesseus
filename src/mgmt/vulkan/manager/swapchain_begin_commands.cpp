#include "manager.h"

core::Result<VkCommandBuffer, core::Status>
mgmt::vulkan::Manager::swapchain_begin_commands(u32 frame_number,
                                                Swapchain::Swapchain& swapchain,
                                                u32& img_idx)
{
  // we assume we are init
  auto current_frame = swapchain.get_frame(frame_number);
  // wait and prepare for next render
  auto status = check(vkWaitForFences(device_,
                                      1,
                                      &current_frame.render_fence,
                                      true,
                                      1000000000)); // 1s
  if (status != core::Status::SUCCESS) {
    logger.log("ready_swapchain failed awaiting render fence");
    return core::Status::ERROR;
  }
  current_frame.del_queue.flush();
  status = check(vkResetFences(device_, 1, &current_frame.render_fence));
  if (status != core::Status::SUCCESS) {
    logger.log("ready_swapchain failed resetting fence");
    return core::Status::ERROR;
  }
  status = check(vkAcquireNextImageKHR(device_,
                                       swapchain.swapchain,
                                       1000000000,
                                       current_frame.swapchain_semaphore,
                                       nullptr,
                                       &img_idx));
  if (status != core::Status::SUCCESS) {
    logger.log("ready_swapchain failed acquiring image");
    return core::Status::ERROR;
  }
  VkCommandBuffer cmd = current_frame.main_command_buffer;
  status = check(vkResetCommandBuffer(cmd, 0));
  if (status != core::Status::SUCCESS) {
    logger.log("ready_swapchain failed resetting command buffer");
    return core::Status::ERROR;
  }
  VkCommandBufferBeginInfo cmd_info = info::command_buffer_begin_info(
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  status = check(vkBeginCommandBuffer(cmd, &cmd_info));
  if (status != core::Status::SUCCESS) {
    logger.log("ready_swapchain failed initializing command buffer");
    return core::Status::ERROR;
  }
  return cmd;
}
