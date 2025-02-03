#include "swap-renderer.h"

core::Status
render::SwapRenderer::draw(VkCommandBuffer cmd,
                           u32 img_idx,
                           mgmt::vulkan::Swapchain::Swapchain& swapchain)
{
  // todo@engine: this transition are likely better done outside
  mgmt::vulkan::Image::transition_image(cmd,
                                        swapchain.draw_img.image,
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_GENERAL);
  render::SwapRenderer::ComputeEffect& effect = effects_[current_effect_];
  vkCmdBindPipeline(
    cmd, VK_PIPELINE_BIND_POINT_COMPUTE, effect.pipeline.pipeline);
  // bind the descriptor set containing the draw image for the compute
  vkCmdBindDescriptorSets(cmd,
                          VK_PIPELINE_BIND_POINT_COMPUTE,
                          effect.pipeline.layout,
                          0,
                          1,
                          &swapchain.draw_img_descriptors,
                          0,
                          nullptr);
  vkCmdPushConstants(cmd,
                     effect.pipeline.layout,
                     VK_SHADER_STAGE_COMPUTE_BIT,
                     0,
                     sizeof(ComputePushConstants),
                     &effect.data);
  vkCmdDispatch(cmd,
                std::ceil(swapchain.draw_extent.width / 16.0),
                std::ceil(swapchain.draw_extent.height / 16.0),
                1);
  // todo@engine: these transitions and copy are likely better done outside
  mgmt::vulkan::Image::transition_image(cmd,
                                        swapchain.draw_img.image,
                                        VK_IMAGE_LAYOUT_GENERAL,
                                        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  mgmt::vulkan::Image::transition_image(cmd,
                                        swapchain.imgs[img_idx],
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  mgmt::vulkan::Image::copy_image(cmd,
                                  swapchain.draw_img.image,
                                  swapchain.imgs[img_idx],
                                  swapchain.draw_extent,
                                  swapchain.extent);
  return core::Status::SUCCESS;
};
