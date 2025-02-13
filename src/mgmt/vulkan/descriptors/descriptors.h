#pragma once

#include "../manager.h"

#include <span>

namespace mgmt {
namespace vulkan {

namespace descriptor {

struct LayoutBuilder
{
  std::vector<VkDescriptorSetLayoutBinding> bindings;

  core::code add_binding(u32 binding, VkDescriptorType type);
  core::code clear();
  core::Result<VkDescriptorSetLayout, core::code> build(
    VkDevice device,
    VkShaderStageFlags shader_stages,
    void* p_next = nullptr,
    VkDescriptorSetLayoutCreateFlags flags = 0);
};

struct Writer
{
  std::deque<VkDescriptorImageInfo> image_infos;
  std::deque<VkDescriptorBufferInfo> buffer_infos;
  std::vector<VkWriteDescriptorSet> write_sets;

  void write_image(i32 binding,
                   VkImageView image,
                   VkSampler sampler,
                   VkImageLayout layout,
                   VkDescriptorType type);
  void write_buffer(i32 binding,
                    VkBuffer buffer,
                    size_t size,
                    size_t offset,
                    VkDescriptorType type);
  void clear();
  void update_set(VkDevice device, VkDescriptorSet set);
};

struct PoolSizeRatio
{
  VkDescriptorType type;
  float ratio;
};

struct StaticAllocator
{
  VkDescriptorPool pool;

  core::code init_pool(VkDevice device,
                       u32 max_sets,
                       std::span<PoolSizeRatio> pool_ratios);
  core::code destroy_pool(VkDevice device);
  core::code clear(VkDevice device);
  core::Result<VkDescriptorSet, core::code> allocate(
    VkDevice device,
    VkDescriptorSetLayout layout);
};

struct DynamicAllocator
{
  std::vector<PoolSizeRatio> ratios;
  std::vector<VkDescriptorPool> full;
  std::vector<VkDescriptorPool> ready;
  u32 sets_per_pool;

  void init(VkDevice device,
            u32 set_count,
            std::span<PoolSizeRatio> init_ratios);
  void clear_pools(VkDevice device);
  void destroy_pools(VkDevice device);
  VkDescriptorSet allocate(VkDevice device,
                           VkDescriptorSetLayout layout,
                           void* next = nullptr);
  VkDescriptorPool get_pool(VkDevice device);
  VkDescriptorPool create_pool(VkDevice device,
                               u32 set_count,
                               std::span<PoolSizeRatio> create_ratios);
};

} // namespace descriptor

} // namespace vulkan
} // namespace mgmt
