#include "swap-renderer.h"

core::Status
render::SwapRenderer::init(mgmt::vulkan::Swapchain::Swapchain& swapchain)
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
  auto gradient_pipe_result = vk_mgr_->create_pipeline(
    swapchain, layout_info, "./shaders/gradient-color.comp.spv");
  if (!gradient_pipe_result.has_value()) {
    logger_.err("create_pipeline failed to create gradient pipeline");
    return core::Status::ERROR;
  }
  render::SwapRenderer::ComputeEffect gradient = {
    .name = "gradient",
    .pipeline = gradient_pipe_result.value(),
    .data = { .data1 = glm::vec4(1, 0, 0, 1), .data2 = glm::vec4(0, 0, 1, 1) }
  };
  effects_.push_back(gradient);
  // sky renderer init
  auto sky_pipeline_result =
    vk_mgr_->create_pipeline(swapchain, layout_info, "./shaders/sky.comp.spv");
  if (!sky_pipeline_result.has_value()) {
    logger_.err("create_pipeline failed to create sky pipeline");
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
