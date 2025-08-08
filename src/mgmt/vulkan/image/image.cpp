#include "image.h"

core::code
mgmt::vulkan::image::copy_image(VkCommandBuffer cmd,
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
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::image::transition_image(VkCommandBuffer cmd,
                                      VkImage image,
                                      VkImageLayout current,
                                      VkImageLayout next)
{
  VkImageMemoryBarrier2 img_barrier{
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2
  };
  img_barrier.pNext = nullptr;
  img_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  img_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
  img_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  img_barrier.dstAccessMask =
    VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
  img_barrier.oldLayout = current;
  img_barrier.newLayout = next;
  VkImageAspectFlags aspect_mask =
    (next == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
      ? VK_IMAGE_ASPECT_DEPTH_BIT
      : VK_IMAGE_ASPECT_COLOR_BIT;
  img_barrier.subresourceRange =
    mgmt::vulkan::info::image_subresource_range(aspect_mask);
  img_barrier.image = image;
  VkDependencyInfo dependency_info{};
  dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  dependency_info.pNext = nullptr;
  dependency_info.imageMemoryBarrierCount = 1;
  dependency_info.pImageMemoryBarriers = &img_barrier;
  vkCmdPipelineBarrier2(cmd, &dependency_info);
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::image::generate_mipmaps(VkCommandBuffer cmd,
                                      VkImage image,
                                      VkExtent2D extent)
{
  VkImageMemoryBarrier2 imgBarrier = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
    .pNext = nullptr,
    .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
    .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
    .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
    .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
    .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    .srcQueueFamilyIndex = 0,
    .dstQueueFamilyIndex = 0,
    .image = image,
    .subresourceRange =
      info::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT),
  };
  imgBarrier.subresourceRange.levelCount = 1;
  VkDependencyInfo depInfo = { .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                               .pNext = nullptr,
                               .dependencyFlags = 0,
                               .memoryBarrierCount = 0,
                               .pMemoryBarriers = VK_NULL_HANDLE,
                               .bufferMemoryBarrierCount = 0,
                               .pBufferMemoryBarriers = VK_NULL_HANDLE,
                               .imageMemoryBarrierCount = 1,
                               .pImageMemoryBarriers = &imgBarrier };
  VkImageBlit2 blitRegion
  {
    .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
    .pNext = nullptr,
    .srcSubresource = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
    .srcOffsets = {{ 0, 0, 0}, {0, 0, 1}},
    .dstSubresource = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
    .dstOffsets = {{ 0, 0, 0}, {0, 0, 1}},
  };
  VkBlitImageInfo2 blitInfo = {
    .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
    .pNext = nullptr,
    .srcImage = image,
    .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    .dstImage = image,
    .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    .regionCount = 1,
    .pRegions = &blitRegion,
    .filter = VK_FILTER_LINEAR,
  };
  i32 levels =
    i32(std::floor(std::log2(std::max(extent.width, extent.height)))) + 1;
  for (i32 mip = 0; mip < levels - 1; ++mip) {
    VkExtent2D half = extent;
    half.width /= 2;
    half.height /= 2;
    imgBarrier.subresourceRange.baseMipLevel = mip;
    vkCmdPipelineBarrier2(cmd, &depInfo);
    blitRegion.srcOffsets[1].x = extent.width;
    blitRegion.srcOffsets[1].y = extent.height;
    blitRegion.dstOffsets[1].x = half.width;
    blitRegion.dstOffsets[1].y = half.height;
    blitRegion.srcSubresource.mipLevel = mip;
    blitRegion.dstSubresource.mipLevel = mip + 1;
    vkCmdBlitImage2(cmd, &blitInfo);
    extent = half;
  }
  if (levels > 0) {
    VkExtent2D half = extent;
    half.width /= 2;
    half.height /= 2;
    imgBarrier.subresourceRange.baseMipLevel = levels - 1;
    vkCmdPipelineBarrier2(cmd, &depInfo);
  }
  transition_image(cmd,
                   image,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
