#include "info.h"

VkFenceCreateInfo
mgmt::vulkan::Info::fence_create_info(VkFenceCreateFlags flags)
{
  VkFenceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = flags;
  return info;
}
