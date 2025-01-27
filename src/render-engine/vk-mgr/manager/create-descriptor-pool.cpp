#include "manager.h"

using namespace RenderEngine;
using namespace RenderEngine::VulkanManager;

core::Result<VkDescriptorPool, Status>
Manager::create_descriptor_pool(VkDescriptorPoolCreateInfo pool_info)
{
  VkDescriptorPool pool;
  auto status =
    check(vkCreateDescriptorPool(device_, &pool_info, nullptr, &pool));
  if (status != Status::SUCCESS) {
    logger.error("create_descriptor_pool failed, vkCreateDescriptorPool error");
    return status;
  }
  del_queue_.push([=]() { vkDestroyDescriptorPool(device_, pool, nullptr); });
  return pool;
}
