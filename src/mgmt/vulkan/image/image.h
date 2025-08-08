#pragma once

#include "../info/info.h"
#include "../manager.h"

namespace mgmt {
namespace vulkan {

namespace image {

struct AllocatedImage
{
  VkImage image;
  VkImageView view;
  VkFormat format;
  VkExtent3D extent_3d;
  VkExtent2D extent_2d;
  VmaAllocation allocation;
};

core::code
transition_image(VkCommandBuffer cmd,
                 VkImage image,
                 VkImageLayout current,
                 VkImageLayout next);

core::code
copy_image(VkCommandBuffer cmd,
           VkImage source,
           VkImage destination,
           VkExtent2D src_size,
           VkExtent2D dst_size);

core::code
generate_mipmaps(VkCommandBuffer cmd, VkImage image, VkExtent2D extent);

} // namespace image

} // namespace vulkan
} // namespace mgmt
