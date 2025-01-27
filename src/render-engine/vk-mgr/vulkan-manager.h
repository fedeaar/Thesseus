#pragma once

#include "../render-engine.h"
#include "./descriptors/descriptors.h"
#include "./image/image.h"
#include "./info/info.h"
#include "./manager/manager.h"
#include "./pipeline/pipeline.h"

namespace RenderEngine {
namespace VulkanManager {

static inline std::string const&
namespace_()
{
  return RenderEngine::namespace_() + "::VulkanManager";
};

core::Logger logger{ namespace_() };
Status
check(VkResult result)
{
  if (result != 0) {
    logger.error(string_VkResult(result));
    return Status::ERROR;
  }
  return Status::SUCCESS;
};

} // namespace VulkanManager
} // namespace RenderEngine
