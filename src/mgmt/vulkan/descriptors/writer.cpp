#include "descriptors.h"

void
mgmt::vulkan::descriptor::Writer::write_buffer(i32 binding,
                                               VkBuffer buffer,
                                               size_t size,
                                               size_t offset,
                                               VkDescriptorType type)
{
  VkDescriptorBufferInfo& info =
    buffer_infos.emplace_back(VkDescriptorBufferInfo{
      .buffer = buffer, .offset = offset, .range = size });
  VkWriteDescriptorSet write = { .sType =
                                   VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
  write.dstBinding = binding;
  write.dstSet = VK_NULL_HANDLE;
  write.descriptorCount = 1;
  write.descriptorType = type;
  write.pBufferInfo = &info;
  write_sets.push_back(write);
}

void
mgmt::vulkan::descriptor::Writer::write_image(i32 binding,
                                              VkImageView image,
                                              VkSampler sampler,
                                              VkImageLayout layout,
                                              VkDescriptorType type)
{
  VkDescriptorImageInfo& info = image_infos.emplace_back(VkDescriptorImageInfo{
    .sampler = sampler, .imageView = image, .imageLayout = layout });
  VkWriteDescriptorSet write = { .sType =
                                   VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
  write.dstBinding = binding;
  write.dstSet = VK_NULL_HANDLE;
  write.descriptorCount = 1;
  write.descriptorType = type;
  write.pImageInfo = &info;
  write_sets.push_back(write);
};

void
mgmt::vulkan::descriptor::Writer::clear()
{
  image_infos.clear();
  write_sets.clear();
  buffer_infos.clear();
};

void
mgmt::vulkan::descriptor::Writer::update_set(VkDevice device,
                                             VkDescriptorSet set)
{
  for (VkWriteDescriptorSet& write : write_sets) {
    write.dstSet = set;
  }
  vkUpdateDescriptorSets(
    device, (u32)write_sets.size(), write_sets.data(), 0, nullptr);
};
