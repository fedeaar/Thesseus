#include "vkimage.h"

void vkutil::transition_image(VkCommandBuffer cmd, VkImage image,
                              VkImageLayout current, VkImageLayout next) {
  VkImageMemoryBarrier2 img_barrier{
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
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
  img_barrier.subresourceRange = vkinit::image_subresource_range(aspect_mask);
  img_barrier.image = image;
  VkDependencyInfo dependency_info{};
  dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  dependency_info.pNext = nullptr;
  dependency_info.imageMemoryBarrierCount = 1;
  dependency_info.pImageMemoryBarriers = &img_barrier;
  vkCmdPipelineBarrier2(cmd, &dependency_info);
}