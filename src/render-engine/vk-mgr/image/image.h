#pragma once

#include "../vulkan-manager.h"

namespace RenderEngine {
namespace VulkanManager {

namespace Image {

Status
transition_image(VkCommandBuffer cmd,
                 VkImage image,
                 VkImageLayout current,
                 VkImageLayout next);

Status
copy_image(VkCommandBuffer cmd,
           VkImage source,
           VkImage destination,
           VkExtent2D src_size,
           VkExtent2D dst_size);
} // namespace Image

} // namespace VulkanManager
} // namespace RenderEngine
