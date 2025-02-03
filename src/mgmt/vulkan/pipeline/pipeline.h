#pragma once

#include "../manager.h"

namespace mgmt {
namespace vulkan {

namespace Pipeline {

struct Pipeline
{
  VkPipeline pipeline;
  VkPipelineLayout layout;
};

core::Result<VkShaderModule, core::Status>
load_shader_module(const char* file_path, VkDevice device);
}

} // namespace vulkan
} // namespace mgmt
