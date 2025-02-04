#include "swap-renderer.h"

core::Status
render::SwapRenderer::draw(VkCommandBuffer cmd,
                           u32 img_idx,
                           mgmt::vulkan::Swapchain::Swapchain& swapchain)
{
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
  return core::Status::SUCCESS;
};
