#include "renderer-swap.h"

using namespace RenderEngine;

SwapRenderer::ResultStatus
SwapRenderer::create_pipeline()
{
  // todo@engine: other errors
  // layout
  VkPipelineLayoutCreateInfo layout_info{};
  layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layout_info.pNext = nullptr;
  layout_info.pSetLayouts =
    &engine_->swapchain_.params_.draw_img_descriptor_layout_;
  layout_info.setLayoutCount = 1;
  VkPushConstantRange pushConstant{};
  pushConstant.offset = 0;
  pushConstant.size = sizeof(ComputePushConstants);
  pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  layout_info.pPushConstantRanges = &pushConstant;
  layout_info.pushConstantRangeCount = 1;
  // gradient init
  auto gradient_pipe_result = manager_->create_pipeline(
    engine_->swapchain_, layout_info, "./shaders/gradient-color.comp.spv");
  if (!gradient_pipe_result.has_value()) {
    RENDERER_ERROR("failed to create gradient pipeline");
    return ResultStatus::ERROR;
  }
  ComputeEffect gradient = { .name = "gradient",
                             .pipeline = gradient_pipe_result.value(),
                             .data = { .data1 = glm::vec4(1, 0, 0, 1),
                                       .data2 = glm::vec4(0, 0, 1, 1) } };
  effects_.push_back(gradient);
  // sky renderer init
  auto sky_pipeline_result = manager_->create_pipeline(
    engine_->swapchain_, layout_info, "./shaders/sky.comp.spv");
  if (!sky_pipeline_result.has_value()) {
    RENDERER_ERROR("failed to create sky pipeline");
    return ResultStatus::ERROR;
  }
  ComputeEffect sky = { .name = "sky",
                        .pipeline = sky_pipeline_result.value(),
                        .data = { .data1 = glm::vec4(0.1, 0.2, 0.4, 0.97) } };
  // swap renderer
  effects_.push_back(sky);
  return ResultStatus::SUCCESS;
}
