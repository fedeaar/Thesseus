#include "image.h"

core::Status
mgmt::vulkan::Image::copy_image(VkCommandBuffer cmd,
                                VkImage source,
                                VkImage destination,
                                VkExtent2D src_size,
                                VkExtent2D dst_size)
{
  VkImageBlit2 blit_region{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
                            .pNext = nullptr };
  blit_region.srcOffsets[1].x = src_size.width;
  blit_region.srcOffsets[1].y = src_size.height;
  blit_region.srcOffsets[1].z = 1;
  blit_region.dstOffsets[1].x = dst_size.width;
  blit_region.dstOffsets[1].y = dst_size.height;
  blit_region.dstOffsets[1].z = 1;
  blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  blit_region.srcSubresource.baseArrayLayer = 0;
  blit_region.srcSubresource.layerCount = 1;
  blit_region.srcSubresource.mipLevel = 0;
  blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  blit_region.dstSubresource.baseArrayLayer = 0;
  blit_region.dstSubresource.layerCount = 1;
  blit_region.dstSubresource.mipLevel = 0;
  VkBlitImageInfo2 blit_info{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
                              .pNext = nullptr };
  blit_info.dstImage = destination;
  blit_info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  blit_info.srcImage = source;
  blit_info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  blit_info.filter = VK_FILTER_LINEAR;
  blit_info.regionCount = 1;
  blit_info.pRegions = &blit_region;
  vkCmdBlitImage2(cmd, &blit_info);
  return core::Status::SUCCESS;
}
