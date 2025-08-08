#pragma once

#include "../info/info.h"
#include "../manager.h"

namespace mgmt {
namespace vulkan {

namespace buffer {

struct AllocatedBuffer
{
  VkBuffer buffer;
  VmaAllocation allocation;
  VmaAllocationInfo info;
};

} // namespace buffer

} // namespace vulkan
} // namespace mgmt
