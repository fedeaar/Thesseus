#include "info.h"

//
// command
//

VkCommandBufferAllocateInfo
mgmt::vulkan::info::command_buffer_allocate_info(VkCommandPool pool, u32 count)
{
  VkCommandBufferAllocateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.pNext = nullptr;
  info.commandPool = pool;
  info.commandBufferCount = count;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  return info;
}

VkCommandBufferBeginInfo
mgmt::vulkan::info::command_buffer_begin_info(VkCommandBufferUsageFlags flags)
{
  VkCommandBufferBeginInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  info.pNext = nullptr;
  info.pInheritanceInfo = nullptr;
  info.flags = flags;
  return info;
}

VkCommandBufferSubmitInfo
mgmt::vulkan::info::command_buffer_submit_info(VkCommandBuffer cmd)
{
  VkCommandBufferSubmitInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
  info.pNext = nullptr;
  info.commandBuffer = cmd;
  info.deviceMask = 0;
  return info;
}

VkCommandPoolCreateInfo
mgmt::vulkan::info::command_pool_create_info(u32 queue_family_idx,
                                             VkCommandPoolCreateFlags flags)
{
  VkCommandPoolCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.pNext = nullptr;
  info.queueFamilyIndex = queue_family_idx;
  info.flags = flags;
  return info;
}

VkSubmitInfo2
mgmt::vulkan::info::submit_info(VkCommandBufferSubmitInfo* cmd,
                                VkSemaphoreSubmitInfo* signal_semaphore_info,
                                VkSemaphoreSubmitInfo* wait_semaphore_info)
{
  VkSubmitInfo2 info = {};
  info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
  info.pNext = nullptr;
  info.waitSemaphoreInfoCount = wait_semaphore_info == nullptr ? 0 : 1;
  info.pWaitSemaphoreInfos = wait_semaphore_info;
  info.signalSemaphoreInfoCount = signal_semaphore_info == nullptr ? 0 : 1;
  info.pSignalSemaphoreInfos = signal_semaphore_info;
  info.commandBufferInfoCount = 1;
  info.pCommandBufferInfos = cmd;
  return info;
}

//
// sync
//

VkFenceCreateInfo
mgmt::vulkan::info::fence_create_info(VkFenceCreateFlags flags)
{
  VkFenceCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = flags;
  return info;
}

VkSemaphoreCreateInfo
mgmt::vulkan::info::semaphore_create_info(VkSemaphoreCreateFlags flags)
{
  VkSemaphoreCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = flags;
  return info;
}

VkSemaphoreSubmitInfo
mgmt::vulkan::info::semaphore_submit_info(VkPipelineStageFlags2 stage_mask,
                                          VkSemaphore semaphore)
{
  VkSemaphoreSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
  submit_info.pNext = nullptr;
  submit_info.semaphore = semaphore;
  submit_info.stageMask = stage_mask;
  submit_info.deviceIndex = 0;
  submit_info.value = 1;
  return submit_info;
}

//
// image
//

VkImageCreateInfo
mgmt::vulkan::info::image_create_info(VkFormat format,
                                      VkImageUsageFlags usage_flags,
                                      VkExtent3D extent)
{
  VkImageCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.pNext = nullptr;
  info.imageType = VK_IMAGE_TYPE_2D;
  info.format = format;
  info.extent = extent;
  info.mipLevels = 1;
  info.arrayLayers = 1;
  info.samples = VK_SAMPLE_COUNT_1_BIT;
  info.tiling = VK_IMAGE_TILING_OPTIMAL;
  info.usage = usage_flags;
  return info;
}

VkImageSubresourceRange
mgmt::vulkan::info::image_subresource_range(VkImageAspectFlags aspect_mask)
{
  VkImageSubresourceRange range{};
  range.aspectMask = aspect_mask;
  range.baseMipLevel = 0;
  range.levelCount = VK_REMAINING_MIP_LEVELS;
  range.baseArrayLayer = 0;
  range.layerCount = VK_REMAINING_ARRAY_LAYERS;
  return range;
}

VkImageViewCreateInfo
mgmt::vulkan::info::imageview_create_info(VkFormat format,
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

//
// render
//

VkRenderingAttachmentInfo
mgmt::vulkan::info::color_attachment_info(VkImageView view,
                                          VkClearValue* clear,
                                          VkImageLayout layout)
{
  VkRenderingAttachmentInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  info.pNext = nullptr;
  info.imageView = view;
  info.imageLayout = layout;
  info.loadOp =
    clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
  info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  if (clear) {
    info.clearValue = *clear;
  }

  return info;
}

VkRenderingAttachmentInfo
mgmt::vulkan::info::depth_attachment_info(VkImageView view,
                                          VkImageLayout layout)
{
  VkRenderingAttachmentInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  info.pNext = nullptr;
  info.imageView = view;
  info.imageLayout = layout;
  info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  info.clearValue.depthStencil.depth = 1.f;
  return info;
}

VkRenderingInfo
mgmt::vulkan::info::rendering_info(
  VkExtent2D render_extent,
  VkRenderingAttachmentInfo* color_attachment_info,
  VkRenderingAttachmentInfo* depth_attachment_info)
{
  VkRenderingInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
  info.pNext = nullptr;
  info.renderArea = VkRect2D{ VkOffset2D{ 0, 0 }, render_extent };
  info.layerCount = 1;
  info.colorAttachmentCount = 1;
  info.pColorAttachments = color_attachment_info;
  info.pDepthAttachment = depth_attachment_info;
  info.pStencilAttachment = nullptr;
  return info;
}

//
// pipeline
//

VkPipelineLayoutCreateInfo
mgmt::vulkan::info::pipeline_layout_create_info()
{
  VkPipelineLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.setLayoutCount = 0;
  info.pSetLayouts = nullptr;
  info.pushConstantRangeCount = 0;
  info.pPushConstantRanges = nullptr;
  return info;
}

VkPipelineShaderStageCreateInfo
mgmt::vulkan::info::pipeline_shader_stage_create_info(
  VkShaderStageFlagBits stage,
  VkShaderModule module,
  const char* entry)
{
  VkPipelineShaderStageCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.pNext = nullptr;
  info.stage = stage;
  info.module = module;
  info.pName = entry;
  return info;
}
