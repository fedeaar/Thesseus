#include "info.h"

VkCommandPoolCreateInfo
RenderEngine::VulkanManager::Info::command_pool_create_info(
  u32 queue_family_idx,
  VkCommandPoolCreateFlags flags = 0)
{
  VkCommandPoolCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.pNext = nullptr;
  info.queueFamilyIndex = queue_family_idx;
  info.flags = flags;
  return info;
}
