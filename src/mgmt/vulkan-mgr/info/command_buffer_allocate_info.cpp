#include "info.h"

VkCommandBufferAllocateInfo
ResourceManagement::VulkanManager::Info::command_buffer_allocate_info(
  VkCommandPool pool,
  u32 count)
{
  VkCommandBufferAllocateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.pNext = nullptr;
  info.commandPool = pool;
  info.commandBufferCount = count;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  return info;
}
