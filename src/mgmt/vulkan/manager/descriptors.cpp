#include "manager.h"

//
// create
//

core::Result<VkDescriptorPool, core::code>
mgmt::vulkan::Manager::create_descriptor_pool(
  VkDescriptorPoolCreateInfo pool_info)
{
  if (!initialized) {
    logger.err("create_descriptor_pool failed, Manager not initialized");
    return core::code::NOT_INIT;
  }
  VkDescriptorPool pool;
  auto status =
    check(vkCreateDescriptorPool(device_, &pool_info, nullptr, &pool));
  if (status != core::code::SUCCESS) {
    logger.err("create_descriptor_pool failed, vkCreateDescriptorPool error");
    return status;
  }
  del_queue_.push([=]() { // TODO@engine: no need to keep pool existing here ?
    vkDeviceWaitIdle(device_);
    vkDestroyDescriptorPool(device_, pool, nullptr);
  });
  return pool;
}
