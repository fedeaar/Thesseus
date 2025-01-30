#include "vulkan-manager.h"

std::string const& ResourceManagement::VulkanManager::namespace_ =
  ResourceManagement::namespace_ + "::VulkanManager";

core::Logger ResourceManagement::VulkanManager::logger{ namespace_ };

ResourceManagement::Status
ResourceManagement::VulkanManager::check(VkResult result)
{
  if (result != 0) {
    logger.error(string_VkResult(result));
    return ResourceManagement::Status::ERROR;
  }
  return ResourceManagement::Status::SUCCESS;
};
