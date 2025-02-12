#include "descriptors.h"

core::Status
mgmt::vulkan::descriptor::StaticAllocator::init_pool(
  VkDevice device,
  u32 max_sets,
  std::span<PoolSizeRatio> ratios)
{
  std::vector<VkDescriptorPoolSize> pool_sizes;
  for (PoolSizeRatio ratio : ratios) {
    pool_sizes.push_back(VkDescriptorPoolSize{
      .type = ratio.type, .descriptorCount = (u32)(ratio.ratio * max_sets) });
  }
  VkDescriptorPoolCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO
  };
  info.flags = 0;
  info.maxSets = max_sets;
  info.poolSizeCount = (u32)pool_sizes.size();
  info.pPoolSizes = pool_sizes.data();
  return check(vkCreateDescriptorPool(device, &info, nullptr, &pool));
}

core::Status
mgmt::vulkan::descriptor::StaticAllocator::destroy_pool(VkDevice device)
{
  vkDestroyDescriptorPool(device, pool, nullptr);
  return core::Status::SUCCESS;
}

core::Status
mgmt::vulkan::descriptor::StaticAllocator::clear(VkDevice device)
{
  return check(vkResetDescriptorPool(device, pool, 0));
}

core::Result<VkDescriptorSet, core::Status>
mgmt::vulkan::descriptor::StaticAllocator::allocate(
  VkDevice device,
  VkDescriptorSetLayout layout)
{
  VkDescriptorSetAllocateInfo info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO
  };
  info.pNext = nullptr;
  info.descriptorPool = pool;
  info.descriptorSetCount = 1;
  info.pSetLayouts = &layout;
  VkDescriptorSet set;
  auto status = check(vkAllocateDescriptorSets(device, &info, &set));
  if (status != core::Status::SUCCESS) {
    return status;
  }
  return set;
}

//
// dynamic allocator
//

void
mgmt::vulkan::descriptor::DynamicAllocator::init(
  VkDevice device,
  u32 set_count,
  std::span<mgmt::vulkan::descriptor::PoolSizeRatio> init_ratios)
{
  ratios.clear();
  for (auto ratio : init_ratios) {
    ratios.push_back(ratio);
  }
  VkDescriptorPool new_pool = create_pool(device, set_count, init_ratios);
  sets_per_pool = set_count * 1.5;
  ready.push_back(new_pool);
}

void
mgmt::vulkan::descriptor::DynamicAllocator::clear_pools(VkDevice device)
{
  for (auto pool : ready) {
    vkResetDescriptorPool(device, pool, 0);
  }
  for (auto pool : full) {
    vkResetDescriptorPool(device, pool, 0);
    ready.push_back(pool);
  }
  full.clear();
};

void
mgmt::vulkan::descriptor::DynamicAllocator::destroy_pools(VkDevice device)
{
  for (auto pool : ready) {
    vkDestroyDescriptorPool(device, pool, nullptr);
  }
  ready.clear();
  for (auto pool : full) {
    vkDestroyDescriptorPool(device, pool, nullptr);
  }
  full.clear();
};

VkDescriptorSet
mgmt::vulkan::descriptor::DynamicAllocator::allocate(
  VkDevice device,
  VkDescriptorSetLayout layout,
  void* next)
{
  VkDescriptorPool pool = get_pool(device);
  VkDescriptorSetAllocateInfo info = {};
  info.pNext = next;
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  info.descriptorPool = pool;
  info.descriptorSetCount = 1;
  info.pSetLayouts = &layout;
  VkDescriptorSet descriptor_set;
  VkResult result = vkAllocateDescriptorSets(device, &info, &descriptor_set);
  if (result == VK_ERROR_OUT_OF_POOL_MEMORY ||
      result == VK_ERROR_FRAGMENTED_POOL) {
    full.push_back(pool);
    pool = get_pool(device);
    info.descriptorPool = pool;
    auto result =
      check(vkAllocateDescriptorSets(device, &info, &descriptor_set));
    // TODO handle error
  }
  ready.push_back(pool);
  return descriptor_set;
};

VkDescriptorPool
mgmt::vulkan::descriptor::DynamicAllocator::get_pool(VkDevice device)
{
  VkDescriptorPool new_pool;
  if (ready.size() != 0) {
    new_pool = ready.back();
    ready.pop_back();
  } else {
    new_pool = create_pool(device, sets_per_pool, ratios);
    sets_per_pool = sets_per_pool * 1.5;
    if (sets_per_pool > 4092) {
      sets_per_pool = 4092;
    }
  }
  return new_pool;
};

VkDescriptorPool
mgmt::vulkan::descriptor::DynamicAllocator::create_pool(
  VkDevice device,
  u32 set_count,
  std::span<mgmt::vulkan::descriptor::PoolSizeRatio> create_ratios)
{
  std::vector<VkDescriptorPoolSize> sizes;
  for (PoolSizeRatio ratio : create_ratios) {
    sizes.push_back(VkDescriptorPoolSize{
      .type = ratio.type, .descriptorCount = u32(ratio.ratio * set_count) });
  }
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = 0;
  pool_info.maxSets = set_count;
  pool_info.poolSizeCount = (uint32_t)sizes.size();
  pool_info.pPoolSizes = sizes.data();
  VkDescriptorPool new_pool;
  vkCreateDescriptorPool(device, &pool_info, nullptr, &new_pool);
  return new_pool;
};
