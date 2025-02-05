#pragma once

#include "manager.h"

core::Status
mgmt::vulkan::Manager::imm_submit(
  std::function<void(VkCommandBuffer cmd)>&& function)
{
  auto status = check(vkResetFences(device_, 1, &imm_submit_.fence));
  if (status != core::Status::SUCCESS) { // todo@engine: add err logs
    return core::Status::ERROR;
  }
  status = check(vkResetCommandBuffer(imm_submit_.command_buffer, 0));
  if (status != core::Status::SUCCESS) {
    return core::Status::ERROR;
  }
  VkCommandBuffer cmd = imm_submit_.command_buffer;
  VkCommandBufferBeginInfo cmdBeginInfo = info::command_buffer_begin_info(
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  status = check(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
  if (status != core::Status::SUCCESS) {
    return core::Status::ERROR;
  }
  function(cmd);
  status = check(vkEndCommandBuffer(cmd));
  if (status != core::Status::SUCCESS) {
    return core::Status::ERROR;
  }
  VkCommandBufferSubmitInfo cmdinfo = info::command_buffer_submit_info(cmd);
  VkSubmitInfo2 submit = info::submit_info(&cmdinfo, nullptr, nullptr);
  status =
    check(vkQueueSubmit2(graphics_queue_, 1, &submit, imm_submit_.fence));
  if (status != core::Status::SUCCESS) {
    return core::Status::ERROR;
  }
  status =
    check(vkWaitForFences(device_,
                          1,
                          &imm_submit_.fence,
                          true,
                          9999999999)); // todo@mgr: use background thread
  if (status != core::Status::SUCCESS) {
    return core::Status::ERROR;
  }
  return core::Status::SUCCESS;
}
