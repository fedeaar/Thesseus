#define VMA_IMPLEMENTATION

#include "manager.h"

core::code
mgmt::vulkan::check(VkResult result)
{
  if (result != 0) {
    core::Logger::err(
      "mgmt::vulkan::check", "vulkan error: {}", string_VkResult(result));
    return core::code::ERROR;
  }
  return core::code::SUCCESS;
};
