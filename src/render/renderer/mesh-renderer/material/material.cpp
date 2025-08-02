#include "material.h"

//
// write material
//

render::asset::material::Instance
render::asset::material::write_material(
  asset::material::Type pass,
  asset::material::MaterialResources const& resources,
  mgmt::vulkan::Manager& vk_mgr,
  mgmt::vulkan::descriptor::DynamicAllocator& descriptor_allocator,
  mgmt::vulkan::descriptor::Writer& writer,
  MaterialPipelines& material_pipes)
{
  auto dev = vk_mgr.get_dev();
  asset::material::Instance data;
  data.type = pass;
  if (pass == asset::material::Type::transparent) {
    data.pipe = &material_pipes.transparent_pipeline;
  } else {
    data.pipe = &material_pipes.opaque_pipeline;
  }
  data.material_set = descriptor_allocator.allocate(dev, material_pipes.layout);
  writer.clear();
  writer.write_buffer(0,
                      resources.data_buff,
                      sizeof(asset::material::GPUMaterialConstants),
                      resources.data_buff_offset,
                      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  writer.write_image(1,
                     resources.color_img.view,
                     resources.color_sampler,
                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                     VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  writer.write_image(2,
                     resources.metal_img.view,
                     resources.metal_sampler,
                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                     VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  writer.update_set(dev, data.material_set);
  return data;
}
