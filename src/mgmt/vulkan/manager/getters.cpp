#include "manager.h"

VkInstance const&
mgmt::vulkan::Manager::get_instance()
{
  return instance_;
}

VkPhysicalDevice const&
mgmt::vulkan::Manager::get_physical_dev()
{
  return gpu_;
}

VkDevice const&
mgmt::vulkan::Manager::get_dev()
{
  return device_;
}

VkQueue const&
mgmt::vulkan::Manager::get_graphics_queue()
{
  return graphics_queue_;
}
