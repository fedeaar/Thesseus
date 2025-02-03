#pragma once

#include "../manager.h"

namespace mgmt {
namespace vulkan {

namespace Info {

// command
VkCommandPoolCreateInfo
command_pool_create_info(u32 queue_family_idx,
                         VkCommandPoolCreateFlags flags = 0);

VkCommandBufferAllocateInfo
command_buffer_allocate_info(VkCommandPool pool, u32 count = 1);

VkCommandBufferBeginInfo
command_buffer_begin_info(VkCommandBufferUsageFlags flags = 0);

VkCommandBufferSubmitInfo
command_buffer_submit_info(VkCommandBuffer cmd);

VkSubmitInfo2
submit_info(VkCommandBufferSubmitInfo* cmd,
            VkSemaphoreSubmitInfo* signal_semaphore_info,
            VkSemaphoreSubmitInfo* wait_semaphore_info);

// sync
VkFenceCreateInfo
fence_create_info(VkFenceCreateFlags flags = 0);

VkSemaphoreCreateInfo
semaphore_create_info(VkSemaphoreCreateFlags flags = 0);

VkSemaphoreSubmitInfo
semaphore_submit_info(VkPipelineStageFlags2 stage_mask, VkSemaphore semaphore);

// image
VkImageSubresourceRange
image_subresource_range(VkImageAspectFlags aspect_mask);

VkImageCreateInfo
image_create_info(VkFormat format,
                  VkImageUsageFlags usage_flags,
                  VkExtent3D extent);

VkImageViewCreateInfo
imageview_create_info(VkFormat format,
                      VkImage image,
                      VkImageAspectFlags aspect_flags);

// render
VkRenderingAttachmentInfo
rendering_attachment_info(VkImageView view,
                          VkClearValue* clear,
                          VkImageLayout layout);

VkRenderingInfo
rendering_info(VkExtent2D render_extent,
               VkRenderingAttachmentInfo* color_attachment_info,
               VkRenderingAttachmentInfo* depth_attachment_info);
} // namespace Info

} // namespace vulkan
} // namespace mgmt
