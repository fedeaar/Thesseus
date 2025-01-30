#include "manager.h"

core::Result<VkDescriptorPool, ResourceManagement::Status>
ResourceManagement::VulkanManager::Manager::create_descriptor_pool(
  VkDescriptorPoolCreateInfo pool_info)
{
  if (!initialized) {
    logger.error("create_descriptor_pool failed, Manager not initialized");
    return ResourceManagement::Status::NOT_INIT;
  }
  VkDescriptorPool pool;
  auto status =
    check(vkCreateDescriptorPool(device_, &pool_info, nullptr, &pool));
  if (status != ResourceManagement::Status::SUCCESS) {
    logger.error("create_descriptor_pool failed, vkCreateDescriptorPool error");
    return status;
  }
  del_queue_.push([=]() { vkDestroyDescriptorPool(device_, pool, nullptr); });
  return pool;
}
