#include "info.h"

VkRenderingInfo
mgmt::vulkan::Info::rendering_info(
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
