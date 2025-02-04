#include "renderer.h"

//
// constructor
//

core::Status
render::SwapRenderer::init(mgmt::vulkan::swapchain::Swapchain& swapchain)
{
  if (initialized) {
    return core::Status::SUCCESS;
  }
  swapchain_ = swapchain;
  // layout
  VkPipelineLayoutCreateInfo layout_info{};
  layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layout_info.pNext = nullptr;
  layout_info.pSetLayouts = &swapchain.draw_img_descriptor_layout;
  layout_info.setLayoutCount = 1;
  VkPushConstantRange push_consts{};
  push_consts.offset = 0;
  push_consts.size = sizeof(ComputePushConstants);
  push_consts.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  layout_info.pPushConstantRanges = &push_consts;
  layout_info.pushConstantRangeCount = 1;
  // gradient init
  auto gradient_pipe_result = vk_mgr_->create_compute_pipeline(
    swapchain, layout_info, "./shaders/gradient-color.comp.spv");
  if (!gradient_pipe_result.has_value()) {
    logger_.err("create_compute_pipeline failed to create gradient pipeline");
    return core::Status::ERROR;
  }
  render::SwapRenderer::ComputeEffect gradient = {
    .name = "gradient",
    .pipeline = gradient_pipe_result.value(),
    .data = { .data1 = glm::vec4(1, 0, 0, 1), .data2 = glm::vec4(0, 0, 1, 1) }
  };
  effects_.push_back(gradient);
  // sky renderer init
  auto sky_pipeline_result = vk_mgr_->create_compute_pipeline(
    swapchain, layout_info, "./shaders/sky.comp.spv");
  if (!sky_pipeline_result.has_value()) {
    logger_.err("create_compute_pipeline failed to create sky pipeline");
    return core::Status::ERROR;
  }
  render::SwapRenderer::ComputeEffect sky = {
    .name = "sky",
    .pipeline = sky_pipeline_result.value(),
    .data = { .data1 = glm::vec4(0.1, 0.2, 0.4, 0.97) }
  };
  // swap renderer
  effects_.push_back(sky);
  // success
  initialized = true;
  return core::Status::SUCCESS;
}

render::SwapRenderer::SwapRenderer(mgmt::vulkan::Manager* vk_mgr)
  : render::Renderer{ vk_mgr } {};

//
// destructor
//

core::Status
render::SwapRenderer::destroy()
{
  // todo@engine: handle pipes?
  initialized = false;
  return core::Status::SUCCESS;
}

render::SwapRenderer::~SwapRenderer()
{
  if (initialized) {
    destroy();
  }
}

//
// draw
//

core::Status
render::SwapRenderer::draw(VkCommandBuffer cmd,
                           u32 img_idx,
                           mgmt::vulkan::swapchain::Swapchain& swapchain)
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
