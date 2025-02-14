#pragma once

#include "../mgmt.h"
#include "../window/manager.h"

#include <VkBootstrap.h>
#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

namespace mgmt {
namespace vulkan {

core::code
check(VkResult const& result);

} // namespace vulkan
} // namespace mgmt
