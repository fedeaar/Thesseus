#include "info.h"

VkImageSubresourceRange
ResourceManagement::VulkanManager::Info::image_subresource_range(
  VkImageAspectFlags aspect_mask)
{
  VkImageSubresourceRange range{};
  range.aspectMask = aspect_mask;
  range.baseMipLevel = 0;
  range.levelCount = VK_REMAINING_MIP_LEVELS;
  range.baseArrayLayer = 0;
  range.layerCount = VK_REMAINING_ARRAY_LAYERS;
  return range;
}
