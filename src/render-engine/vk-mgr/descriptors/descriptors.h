#pragma once

#include "../vulkan-manager.h"

namespace RenderEngine {
namespace VulkanManager {

namespace Descriptor {

struct LayoutBuilder
{
  std::vector<VkDescriptorSetLayoutBinding> bindings;

  Status add_binding(u32 binding, VkDescriptorType type);
  Status clear();
  core::Result<VkDescriptorSetLayout, Status> build(
    VkDevice device,
    VkShaderStageFlags shader_stages,
    void* p_next = nullptr,
    VkDescriptorSetLayoutCreateFlags flags = 0);
};

struct Allocator
{
  struct PoolSizeRatio
  {
    VkDescriptorType type;
    float ratio;
  };
  VkDescriptorPool pool;

  Status init_pool(VkDevice device,
                   u32 max_sets,
                   std::span<PoolSizeRatio> pool_ratios);
  Status destroy_pool(VkDevice device);
  Status clear(VkDevice device);
  core::Result<VkDescriptorSet, Status> allocate(VkDevice device,
                                                 VkDescriptorSetLayout layout);
};
} // namespace Descriptor

} // namespace VulkanManager
} // namespace RenderEngine
