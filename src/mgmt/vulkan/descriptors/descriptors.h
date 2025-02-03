#pragma once

#include "../manager.h"
#include <span>

namespace mgmt {
namespace vulkan {

namespace Descriptor {

struct LayoutBuilder
{
  std::vector<VkDescriptorSetLayoutBinding> bindings;

  core::Status add_binding(u32 binding, VkDescriptorType type);
  core::Status clear();
  core::Result<VkDescriptorSetLayout, core::Status> build(
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

  core::Status init_pool(VkDevice device,
                         u32 max_sets,
                         std::span<PoolSizeRatio> pool_ratios);
  core::Status destroy_pool(VkDevice device);
  core::Status clear(VkDevice device);
  core::Result<VkDescriptorSet, core::Status> allocate(
    VkDevice device,
    VkDescriptorSetLayout layout);
};
} // namespace Descriptor

} // namespace vulkan
} // namespace mgmt
