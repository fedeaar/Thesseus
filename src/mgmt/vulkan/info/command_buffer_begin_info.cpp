#include "info.h"

VkCommandBufferBeginInfo
mgmt::vulkan::Info::command_buffer_begin_info(VkCommandBufferUsageFlags flags)
{
  VkCommandBufferBeginInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  info.pNext = nullptr;
  info.pInheritanceInfo = nullptr;
  info.flags = flags;
  return info;
}
