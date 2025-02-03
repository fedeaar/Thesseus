#include "info.h"

VkCommandPoolCreateInfo
mgmt::vulkan::Info::command_pool_create_info(u32 queue_family_idx,
                                             VkCommandPoolCreateFlags flags)
{
  VkCommandPoolCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.pNext = nullptr;
  info.queueFamilyIndex = queue_family_idx;
  info.flags = flags;
  return info;
}
