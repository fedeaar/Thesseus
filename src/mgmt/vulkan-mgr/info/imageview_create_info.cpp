#include "info.h"

VkImageViewCreateInfo
ResourceManagement::VulkanManager::Info::imageview_create_info(
  VkFormat format,
  VkImage image,
  VkImageAspectFlags aspect_flags)
{
  VkImageViewCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.pNext = nullptr;
  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  info.image = image;
  info.format = format;
  info.subresourceRange.baseMipLevel = 0;
  info.subresourceRange.levelCount = 1;
  info.subresourceRange.baseArrayLayer = 0;
  info.subresourceRange.layerCount = 1;
  info.subresourceRange.aspectMask = aspect_flags;
  return info;
}
