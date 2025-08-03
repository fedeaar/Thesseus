#include "renderer.h"

//
// constructor
//

core::code
render::BackgroundRenderer::init()
{
  if (initialized == core::status::INITIALIZED) {
    return core::code::SUCCESS;
  }
  if (initialized == core::status::ERROR) {
    return core::code::ERROR;
  }
  auto& vkMgr = *p_vkMgr_;
  // layout
  VkPushConstantRange pushConsts{
    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
    .offset = 0,
    .size = sizeof(GPUPushConstants),
  };
  VkPipelineLayoutCreateInfo layoutInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .setLayoutCount = 1,
    .pSetLayouts = &p_swapchain_->draw_img_descriptor_layout,
    .pushConstantRangeCount = 1,
    .pPushConstantRanges = &pushConsts
  };
  { // gradient init
    auto gradientPipeResult = vkMgr.create_compute_pipeline(
      layoutInfo, "./shaders/gradient-color.comp.spv");
    if (!gradientPipeResult.has_value()) {
      ERR("failed to create gradient pipeline");
      return core::code::ERROR;
    }
    render::BackgroundRenderer::ComputeEffect gradient = {
      .name = "gradient",
      .pipeline = gradientPipeResult.value(),
      .data = { .data1 = glm::vec4(1, 0, 0, 1), .data2 = glm::vec4(0, 0, 1, 1) }
    };
    effects_.push_back(gradient);
  }
  { // sky init
    auto sky_pipeline_result =
      vkMgr.create_compute_pipeline(layoutInfo, "./shaders/sky.comp.spv");
    if (!sky_pipeline_result.has_value()) {
      ERR("failed to create sky pipeline");
      return core::code::ERROR;
    }
    render::BackgroundRenderer::ComputeEffect sky = {
      .name = "sky",
      .pipeline = sky_pipeline_result.value(),
      .data = { .data1 = glm::vec4(0.1, 0.2, 0.4, 0.97) }
    };
    effects_.push_back(sky);
  }
  // success
  initialized = core::status::INITIALIZED;
  return core::code::SUCCESS;
}

render::BackgroundRenderer::BackgroundRenderer(
  mgmt::vulkan::Swapchain* mp_swapchain,
  mgmt::vulkan::Manager* mp_vkMgr)
  : render::Renderer{ mp_vkMgr, mp_swapchain } {};

//
// destructor
//

core::code
render::BackgroundRenderer::destroy()
{
  initialized = core::status::NOT_INITIALIZED;
  return core::code::SUCCESS;
}

render::BackgroundRenderer::~BackgroundRenderer()
{
  if (initialized == core::status::INITIALIZED) {
    destroy();
  }
}

//
// draw
//

void
render::BackgroundRenderer::draw()
{
  auto& swapchain = *p_swapchain_;
  auto cmd = swapchain.get_current_cmd_buffer();
  auto& effect = effects_[current_effect_];
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
                     sizeof(GPUPushConstants),
                     &effect.data);
  vkCmdDispatch(cmd,
                std::ceil(swapchain.draw_extent.width / 16.0),
                std::ceil(swapchain.draw_extent.height / 16.0),
                1);
};
