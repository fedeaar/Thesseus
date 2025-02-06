
#include "pipeline.h"

//
// constructor
//

mgmt::vulkan::pipeline::Builder::Builder()
{
  clear();
}

//
// clear
//

void
mgmt::vulkan::pipeline::Builder::clear()
{
  shader_stages_.clear();
  input_assembly_ = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
  };
  rasterizer_ = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
  };
  color_blend_att_ = {};
  multisampling_ = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO
  };
  layout_ = {};
  depth_stencil_ = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO
  };
  rendering_ = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
}

//
// set
//

void
mgmt::vulkan::pipeline::Builder::set_layout(VkPipelineLayout layout)
{
  layout_ = layout;
}

void
mgmt::vulkan::pipeline::Builder::set_shaders(VkShaderModule vs,
                                             VkShaderModule fs)
{
  shader_stages_.clear();
  shader_stages_.push_back(
    mgmt::vulkan::info::pipeline_shader_stage_create_info(
      VK_SHADER_STAGE_VERTEX_BIT, vs));
  shader_stages_.push_back(
    mgmt::vulkan::info::pipeline_shader_stage_create_info(
      VK_SHADER_STAGE_FRAGMENT_BIT, fs));
}

void
mgmt::vulkan::pipeline::Builder::set_input_topology(
  VkPrimitiveTopology topology)
{
  input_assembly_.topology = topology;
  input_assembly_.primitiveRestartEnable = VK_FALSE;
}

void
mgmt::vulkan::pipeline::Builder::set_polygon_mode(VkPolygonMode mode)
{
  rasterizer_.polygonMode = mode;
  rasterizer_.lineWidth = 1.f;
}

void
mgmt::vulkan::pipeline::Builder::set_cull_mode(VkCullModeFlags flags,
                                               VkFrontFace front)
{
  rasterizer_.cullMode = flags;
  rasterizer_.frontFace = front;
}

void
mgmt::vulkan::pipeline::Builder::set_multisampling_none()
{
  multisampling_.sampleShadingEnable = VK_FALSE;
  multisampling_.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling_.minSampleShading = 1.0f;
  multisampling_.pSampleMask = nullptr;
  multisampling_.alphaToCoverageEnable = VK_FALSE;
  multisampling_.alphaToOneEnable = VK_FALSE;
}

void
mgmt::vulkan::pipeline::Builder::set_color_attachment_format(VkFormat format)
{
  color_att_format_ = format;
  rendering_.colorAttachmentCount = 1;
  rendering_.pColorAttachmentFormats = &color_att_format_;
}

void
mgmt::vulkan::pipeline::Builder::set_depth_format(VkFormat format)
{
  rendering_.depthAttachmentFormat = format;
}

void
mgmt::vulkan::pipeline::Builder::enable_blending_additive()
{
  color_blend_att_.colorWriteMask =
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_att_.blendEnable = VK_TRUE;
  color_blend_att_.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_att_.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_att_.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_att_.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_att_.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_att_.alphaBlendOp = VK_BLEND_OP_ADD;
}

void
mgmt::vulkan::pipeline::Builder::enable_blending_alphablend()
{
  color_blend_att_.colorWriteMask =
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_att_.blendEnable = VK_TRUE;
  color_blend_att_.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_att_.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_att_.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_att_.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_att_.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_att_.alphaBlendOp = VK_BLEND_OP_ADD;
}

void
mgmt::vulkan::pipeline::Builder::disable_blending()
{
  color_blend_att_.colorWriteMask =
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_att_.blendEnable = VK_FALSE;
}

void
mgmt::vulkan::pipeline::Builder::enable_depthtest(bool enable, VkCompareOp op)
{
  depth_stencil_.depthTestEnable = VK_TRUE;
  depth_stencil_.depthWriteEnable = enable;
  depth_stencil_.depthCompareOp = op;
  depth_stencil_.depthBoundsTestEnable = VK_FALSE;
  depth_stencil_.stencilTestEnable = VK_FALSE;
  depth_stencil_.front = {};
  depth_stencil_.back = {};
  depth_stencil_.minDepthBounds = 0.f;
  depth_stencil_.maxDepthBounds = 1.f;
}

void
mgmt::vulkan::pipeline::Builder::disable_depthtest()
{
  depth_stencil_.depthTestEnable = VK_FALSE;
  depth_stencil_.depthWriteEnable = VK_FALSE;
  depth_stencil_.depthCompareOp = VK_COMPARE_OP_NEVER;
  depth_stencil_.depthBoundsTestEnable = VK_FALSE;
  depth_stencil_.stencilTestEnable = VK_FALSE;
  depth_stencil_.front = {};
  depth_stencil_.back = {};
  depth_stencil_.minDepthBounds = 0.f;
  depth_stencil_.maxDepthBounds = 1.f;
}

//
// build
//

core::Result<VkPipeline, core::Status>
mgmt::vulkan::pipeline::Builder::build_pipeline(VkDevice device)
{
  // default
  VkPipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.pNext = nullptr;
  viewport_state.viewportCount = 1;
  viewport_state.scissorCount = 1;
  VkPipelineColorBlendStateCreateInfo color_blend = {};
  color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend.pNext = nullptr;
  color_blend.logicOpEnable = VK_FALSE;
  color_blend.logicOp = VK_LOGIC_OP_COPY;
  color_blend.attachmentCount = 1;
  color_blend.pAttachments = &color_blend_att_;
  VkPipelineVertexInputStateCreateInfo vertex_input = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
  };
  VkDynamicState state[] = { VK_DYNAMIC_STATE_VIEWPORT,
                             VK_DYNAMIC_STATE_SCISSOR };

  VkPipelineDynamicStateCreateInfo dynamic_state = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO
  };
  dynamic_state.pDynamicStates = &state[0];
  dynamic_state.dynamicStateCount = 2;
  // setup
  VkGraphicsPipelineCreateInfo pipeline_info = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO
  };
  pipeline_info.pNext = &rendering_;
  pipeline_info.stageCount = (u32)shader_stages_.size();
  pipeline_info.pStages = shader_stages_.data();
  pipeline_info.pVertexInputState = &vertex_input;
  pipeline_info.pInputAssemblyState = &input_assembly_;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer_;
  pipeline_info.pMultisampleState = &multisampling_;
  pipeline_info.pColorBlendState = &color_blend;
  pipeline_info.pDepthStencilState = &depth_stencil_;
  pipeline_info.layout = layout_;
  pipeline_info.pDynamicState = &dynamic_state;
  // build
  VkPipeline gfx_pipeline;
  auto status = check(vkCreateGraphicsPipelines(
    device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &gfx_pipeline));
  if (status != core::Status::SUCCESS) {
    logger.err("build_pipeline failed");
    return core::Status::ERROR;
  }
  return gfx_pipeline;
}
