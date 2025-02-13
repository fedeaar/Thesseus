#include "descriptors.h"

core::code
mgmt::vulkan::descriptor::LayoutBuilder::add_binding(u32 binding,
                                                     VkDescriptorType type)
{
  VkDescriptorSetLayoutBinding bind{};
  bind.binding = binding;
  bind.descriptorCount = 1;
  bind.descriptorType = type;
  bindings.push_back(bind);
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::descriptor::LayoutBuilder::clear()
{
  bindings.clear();
  return core::code::SUCCESS;
}

core::Result<VkDescriptorSetLayout, core::code>
mgmt::vulkan::descriptor::LayoutBuilder::build(
  VkDevice device,
  VkShaderStageFlags shader_stages,
  void* p_next,
  VkDescriptorSetLayoutCreateFlags flags)
{
  for (auto& binding : bindings) {
    binding.stageFlags |= shader_stages;
  }
  VkDescriptorSetLayoutCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO
  };
  info.pNext = p_next;
  info.pBindings = bindings.data();
  info.bindingCount = (u32)bindings.size();
  info.flags = flags;
  VkDescriptorSetLayout set;
  auto status =
    check(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));
  if (status != core::code::SUCCESS) {
    return status;
  }
  return set;
}
