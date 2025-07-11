#pragma once

#include "../renderer.h"

namespace render {
namespace asset {

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
  Type type;
  Pipeline* pipe;
  VkDescriptorSet material_set;
};

struct Material
{
  Instance data;
  u32 count;
  std::shared_ptr<Material> material;
};

} // namespace material

} // namespace asset
} // namespace render
