#include "manager.h"

/*
core::Result<mgmt::vulkan::image::AllocatedImage, core::code>
mgmt::vulkan::Manager::create_image(void* data,
                                    VkExtent3D extent,
                                    VkFormat format,
                                    VkImageUsageFlags usage,
                                    bool mipmapped)
{
  u32 data_size = size.depth * size.width * size.height * 4; // FIXME
  auto buff_result = create_buffer(
    data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
  if (!buff_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::create_image",
                      "create_buffer failed");
    return core::code::ERROR;
  }
  buffer::AllocatedBuffer upload_buff = buff_result.value();
  memcpy(upload_buff.info.pMappedData, data, data_size);
  auto img_result = create_image(size,
                                 format,
                                 usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                   VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                 mipmapped);
  if (!img_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::create_image",
                      "create_image overload failed");
    return core::code::ERROR;
  }
  image::AllocatedImage new_image = img_result.value();
  auto status = imm_submit([&](VkCommandBuffer cmd) {
    image::transition_image(cmd,
                            new_image.image,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    VkBufferImageCopy cpy_region = {};
    cpy_region.bufferOffset = 0;
    cpy_region.bufferRowLength = 0;
    cpy_region.bufferImageHeight = 0;
    cpy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    cpy_region.imageSubresource.mipLevel = 0;
    cpy_region.imageSubresource.baseArrayLayer = 0;
    cpy_region.imageSubresource.layerCount = 1;
    cpy_region.imageExtent = size;
    vkCmdCopyBufferToImage(cmd,
                           upload_buff.buffer,
                           new_image.image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &cpy_region);
    image::transition_image(cmd,
                            new_image.image,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  });
  if (status != core::code::SUCCESS) {
    // TODO: destroy buffer nevertheless
    core::Logger::err("mgmt::vulkan::Manager::create_image",
                      "images setup overload failed");
    return core::code::ERROR;
  }
  status = destroy_buffer(upload_buff);
  if (status != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::create_image",
                      "destroy_buffer failed");
    return core::code::ERROR;
  }
  return new_image;
}

//
// destroy
//

core::code
mgmt::vulkan::Manager::destroy_image(const image::AllocatedImage& img)
{
  vkDestroyImageView(device_, img.view, nullptr);
  vmaDestroyImage(allocator_, img.image, img.allocation);
  return core::code::SUCCESS;
}
 */