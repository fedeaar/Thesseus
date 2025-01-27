#include "renderer-swap.h"

using namespace RenderEngine;

SwapRenderer::ResultStatus
SwapRenderer::draw(VkCommandBuffer cmd)
{
  ComputeEffect& effect = effects_[current_effect_];
  vkCmdBindPipeline(
    cmd, VK_PIPELINE_BIND_POINT_COMPUTE, effect.pipeline.params_.pipeline_);
  // bind the descriptor set containing the draw image for the compute
  vkCmdBindDescriptorSets(cmd,
                          VK_PIPELINE_BIND_POINT_COMPUTE,
                          effect.pipeline.params_.layout_,
                          0,
                          1,
                          &engine_->swapchain_.params_.draw_img_descriptors_,
                          0,
                          nullptr);
  vkCmdPushConstants(cmd,
                     effect.pipeline.params_.layout_,
                     VK_SHADER_STAGE_COMPUTE_BIT,
                     0,
                     sizeof(ComputePushConstants),
                     &effect.data);
  vkCmdDispatch(
    cmd,
    std::ceil(engine_->swapchain_.params_.draw_extent_.width / 16.0),
    std::ceil(engine_->swapchain_.params_.draw_extent_.height / 16.0),
    1);
};
