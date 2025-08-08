#include "types.h"

//
// write_material
//

render::Material::Instance
render::write_material(MaterialPassType pass,
                       MaterialResources const& ir_resources,
                       mgmt::vulkan::Manager& ir_vkMgr,
                       mgmt::vulkan::descriptor::DynamicAllocator& ir_allocator,
                       mgmt::vulkan::descriptor::Writer& ir_writer,
                       MaterialPassPipelines& ir_materialPipes)
{
  auto dev = ir_vkMgr.get_dev();
  Material::Instance data;
  data.type = pass;
  if (pass == MaterialPassType::transparent) {
    data.pipeline = &ir_materialPipes.transparent;
  } else {
    data.pipeline = &ir_materialPipes.opaque;
  }
  data.p_materialSet = ir_allocator.allocate(dev, ir_materialPipes.layout);
  ir_writer.clear();
  ir_writer.write_buffer(0,
                         ir_resources.p_dataBuff,
                         sizeof(GPUMaterialConstants),
                         ir_resources.dataBuffOffset,
                         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  ir_writer.write_image(1,
                        ir_resources.colorImg.view,
                        ir_resources.p_colorSampler,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  ir_writer.write_image(2,
                        ir_resources.metalImg.view,
                        ir_resources.p_metalSampler,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  ir_writer.update_set(dev, data.p_materialSet);
  return data;
}
