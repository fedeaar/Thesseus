#pragma once

#include "../manager.h"

namespace mgmt {
namespace vulkan {

namespace material {

enum Type
{
  opaque,
  transparent,
  other
};

struct Pipeline
{
  VkPipeline pipe;
  VkPipelineLayout layout;
};

struct Instance
{
  Pipeline* pipe;
  VkDescriptorSet material_set;
  Type type;
};

struct Material
{
  Instance data;
  u32 count;
  std::shared_ptr<Material> material;
};

} // namespace material

} // namespace vulkan
} // namespace mgmt
