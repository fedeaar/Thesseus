#include "info.h"

VkSemaphoreCreateInfo
ResourceManagement::VulkanManager::Info::semaphore_create_info(
  VkSemaphoreCreateFlags flags)
{
  VkSemaphoreCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = flags;
  return info;
}
