#include "info.h"

VkCommandBufferSubmitInfo
ResourceManagement::VulkanManager::Info::command_buffer_submit_info(
  VkCommandBuffer cmd)
{
  VkCommandBufferSubmitInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
  info.pNext = nullptr;
  info.commandBuffer = cmd;
  info.deviceMask = 0;
  return info;
}
