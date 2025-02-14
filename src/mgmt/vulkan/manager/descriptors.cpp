#include "manager.h"

//
// create
//

core::Result<VkDescriptorPool, core::code>
mgmt::vulkan::Manager::create_descriptor_pool(
  VkDescriptorPoolCreateInfo pool_info)
{
  if (initialized == core::status::NOT_INIT) {
    core::Logger::err("mgmt::vulkan::Manager::create_descriptor_pool",
                      "manager not initialized");
    return core::code::NOT_INIT;
  }
  if (initialized == core::status::ERROR) {
    core::Logger::err("mgmt::vulkan::Manager::create_descriptor_pool",
                      "manager is in "
                      "error state");
    return core::code::IN_ERROR_STATE;
  }
  VkDescriptorPool pool;
  auto status =
    check(vkCreateDescriptorPool(device_, &pool_info, nullptr, &pool));
  if (status != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::create_descriptor_pool",
                      "vkCreateDescriptorPool error");
    return status;
  }
  del_queue_.push([=]() mutable { destroy_descriptor_pool(pool); });
  return pool;
}

//
// destroy
//

core::code
mgmt::vulkan::Manager::destroy_descriptor_pool(VkDescriptorPool& pool)
{
  device_wait_idle();
  vkDestroyDescriptorPool(device_, pool, nullptr);
}
