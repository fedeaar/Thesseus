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

struct GPUMaterialConstants
{
  v4f color_factors;
  v4f metal_rough_factors;
  // padding
  v4f extra[14];
};

struct MaterialResources
{
  mgmt::vulkan::image::AllocatedImage color_img;
  mgmt::vulkan::image::AllocatedImage metal_img;
  VkSampler color_sampler;
  VkSampler metal_sampler;
  VkBuffer data_buff;
  u32 data_buff_offset;
};

struct MaterialPipelines
{
  Pipeline opaque_pipeline;
  Pipeline transparent_pipeline;
  VkDescriptorSetLayout layout;
};

Instance
write_material(Type pass,
               MaterialResources const& resources,
               mgmt::vulkan::Manager& vk_mgr,
               mgmt::vulkan::descriptor::DynamicAllocator& descriptor_allocator,
               mgmt::vulkan::descriptor::Writer& writer,
               MaterialPipelines& material_pipes);

} // namespace material

} // namespace asset
} // namespace render
