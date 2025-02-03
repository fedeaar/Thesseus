#pragma once

#include "../info/info.h"
#include "../manager.h"

namespace mgmt {
namespace vulkan {

namespace Image {

core::Status
transition_image(VkCommandBuffer cmd,
                 VkImage image,
                 VkImageLayout current,
                 VkImageLayout next);

core::Status
copy_image(VkCommandBuffer cmd,
           VkImage source,
           VkImage destination,
           VkExtent2D src_size,
           VkExtent2D dst_size);
} // namespace Image

} // namespace vulkan
} // namespace mgmt
