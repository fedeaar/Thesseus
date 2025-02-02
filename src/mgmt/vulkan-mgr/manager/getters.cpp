#include "manager.h"

VkInstance const&
ResourceManagement::VulkanManager::Manager::get_instance()
{
  return instance_;
}

VkPhysicalDevice const&
ResourceManagement::VulkanManager::Manager::get_physical_dev()
{
  return gpu_;
}

VkDevice const&
ResourceManagement::VulkanManager::Manager::get_dev()
{
  return device_;
}

VkQueue const&
ResourceManagement::VulkanManager::Manager::get_graphics_queue()
{
  return graphics_queue_;
}
