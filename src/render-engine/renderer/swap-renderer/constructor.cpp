#include "swap-renderer.h"

RenderEngine::Status
RenderEngine::SwapRenderer::init(
  ResourceManagement::VulkanManager::Swapchain::Swapchain& swapchain)
{
  if (initialized) {
    return RenderEngine::Status::SUCCESS;
  }
  swapchain_ = swapchain;
  auto vk_mgr = *vk_mgr_;
  // layout
  VkPipelineLayoutCreateInfo layout_info{};
  layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layout_info.pNext = nullptr;
  layout_info.pSetLayouts = &swapchain.draw_img_descriptor_layout;
  layout_info.setLayoutCount = 1;
  VkPushConstantRange pushConstant{};
  pushConstant.offset = 0;
  pushConstant.size = sizeof(ComputePushConstants);
  pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  layout_info.pPushConstantRanges = &pushConstant;
  layout_info.pushConstantRangeCount = 1;
  // gradient init
  auto gradient_pipe_result = vk_mgr.create_pipeline(
    swapchain, layout_info, "./shaders/gradient-color.comp.spv");
  if (!gradient_pipe_result.has_value()) {
    logger_.error("create_pipeline failed to create gradient pipeline");
    return RenderEngine::Status::ERROR;
  }
  RenderEngine::SwapRenderer::ComputeEffect gradient = {
    .name = "gradient",
    .pipeline = gradient_pipe_result.value(),
    .data = { .data1 = glm::vec4(1, 0, 0, 1), .data2 = glm::vec4(0, 0, 1, 1) }
  };
  effects_.push_back(gradient);
  // sky renderer init
  auto sky_pipeline_result =
    vk_mgr.create_pipeline(swapchain, layout_info, "./shaders/sky.comp.spv");
  if (!sky_pipeline_result.has_value()) {
    logger_.error("create_pipeline failed to create sky pipeline");
    return RenderEngine::Status::ERROR;
  }
  RenderEngine::SwapRenderer::ComputeEffect sky = {
    .name = "sky",
    .pipeline = sky_pipeline_result.value(),
    .data = { .data1 = glm::vec4(0.1, 0.2, 0.4, 0.97) }
  };
  // swap renderer
  effects_.push_back(sky);
  // success
  initialized = true;
  return RenderEngine::Status::SUCCESS;
}

RenderEngine::SwapRenderer::SwapRenderer(
  ResourceManagement::VulkanManager::Manager* vk_mgr)
  : RenderEngine::Renderer{ vk_mgr } {};
