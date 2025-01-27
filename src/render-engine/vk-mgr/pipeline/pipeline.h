#pragma once

#include "../vulkan-manager.h"

namespace RenderEngine {
namespace VulkanManager {

namespace Pipeline {

struct Pipeline
{
  VkPipeline pipeline;
  VkPipelineLayout layout;
};

core::Result<VkShaderModule, Status>
load_shader_module(const char* file_path, VkDevice device);
}

} // namespace VulkanManager
} // namespace RenderEngine
