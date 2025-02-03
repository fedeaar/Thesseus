#include "info.h"

VkSubmitInfo2
mgmt::vulkan::Info::submit_info(VkCommandBufferSubmitInfo* cmd,
                                VkSemaphoreSubmitInfo* signal_semaphore_info,
                                VkSemaphoreSubmitInfo* wait_semaphore_info)
{
  VkSubmitInfo2 info = {};
  info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
  info.pNext = nullptr;
  info.waitSemaphoreInfoCount = wait_semaphore_info == nullptr ? 0 : 1;
  info.pWaitSemaphoreInfos = wait_semaphore_info;
  info.signalSemaphoreInfoCount = signal_semaphore_info == nullptr ? 0 : 1;
  info.pSignalSemaphoreInfos = signal_semaphore_info;
  info.commandBufferInfoCount = 1;
  info.pCommandBufferInfos = cmd;
  return info;
}
