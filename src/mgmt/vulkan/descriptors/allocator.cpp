#include "descriptors.h"

core::Status
mgmt::vulkan::Descriptor::Allocator::init_pool(VkDevice device,
                                               u32 max_sets,
                                               std::span<PoolSizeRatio> ratios)
{
  std::vector<VkDescriptorPoolSize> poolSizes;
  for (PoolSizeRatio ratio : ratios) {
    poolSizes.push_back(VkDescriptorPoolSize{
      .type = ratio.type, .descriptorCount = (u32)(ratio.ratio * max_sets) });
  }
  VkDescriptorPoolCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO
  };
  info.flags = 0;
  info.maxSets = max_sets;
  info.poolSizeCount = (u32)poolSizes.size();
  info.pPoolSizes = poolSizes.data();
  return check(vkCreateDescriptorPool(device, &info, nullptr, &pool));
}

core::Status
mgmt::vulkan::Descriptor::Allocator::destroy_pool(VkDevice device)
{
  vkDestroyDescriptorPool(device, pool, nullptr);
  return core::Status::SUCCESS;
}

core::Status
mgmt::vulkan::Descriptor::Allocator::clear(VkDevice device)
{
  return check(vkResetDescriptorPool(device, pool, 0));
}

core::Result<VkDescriptorSet, core::Status>
mgmt::vulkan::Descriptor::Allocator::allocate(VkDevice device,
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
