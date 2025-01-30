#pragma once

#include "../mgmt.h"
#include "../window-mgr/window-manager.h"

#include <VkBootstrap.h>
#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

namespace ResourceManagement {
namespace VulkanManager {

extern std::string const namespace_;

extern core::Logger logger;

Status
check(VkResult result);

} // namespace VulkanManager
} // namespace ResourceManagement
