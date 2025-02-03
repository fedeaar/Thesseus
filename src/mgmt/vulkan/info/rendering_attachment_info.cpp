#include "info.h"

VkRenderingAttachmentInfo
mgmt::vulkan::Info::rendering_attachment_info(VkImageView view,
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
