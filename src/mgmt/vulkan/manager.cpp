#define VMA_IMPLEMENTATION

#include "manager.h"

std::string const mgmt::vulkan::namespace_ = mgmt::namespace_ + "::vulkan";

core::Logger mgmt::vulkan::logger{ namespace_ };

core::code
mgmt::vulkan::check(VkResult result)
{
  if (result != 0) {
    logger.err("vulkan error: {}", string_VkResult(result));
    return core::code::ERROR;
  }
  return core::code::SUCCESS;
};
