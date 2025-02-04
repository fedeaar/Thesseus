#include "manager.h"

core::Result<mgmt::vulkan::pipeline::Pipeline, core::Status>
mgmt::vulkan::Manager::create_pipeline(Swapchain::Swapchain& swapchain,
                                       VkPipelineLayoutCreateInfo& layout_info,
                                       char* shader_path)
{
  if (!initialized) {
    logger.err("create_pipeline failed, Manager not initialized");
    return core::Status::NOT_INIT;
  }
  Pipeline::Pipeline pipeline;
  // create layout
  auto status = check(
    vkCreatePipelineLayout(device_, &layout_info, nullptr, &pipeline.layout));
  auto load_shader_result = Pipeline::load_shader_module(shader_path, device_);
  if (!load_shader_result.has_value()) {
    logger.err(
      "create_pipeline failed, error when building the compute shader");
    return core::Status::ERROR;
  }
  VkShaderModule shader = load_shader_result.value();
  // create pipeline
  VkPipelineShaderStageCreateInfo stage_info{};
  stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  stage_info.pNext = nullptr;
  stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  stage_info.module = shader;
  stage_info.pName = "main";
  VkComputePipelineCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  create_info.pNext = nullptr;
  create_info.layout = pipeline.layout;
  create_info.stage = stage_info;
  status = check(vkCreateComputePipelines(
    device_, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline.pipeline));
  if (status != core::Status::SUCCESS) {
    logger.err("create_pipeline failed, error when creating compute pipeline");
    return status;
  }
  // set destroyer
  vkDestroyShaderModule(device_, shader, nullptr);
  del_queue_.push([=]() {
    vkDeviceWaitIdle(device_);
    vkDestroyPipelineLayout(device_, pipeline.layout, nullptr);
    vkDestroyPipeline(device_, pipeline.pipeline, nullptr);
  });
  return pipeline;
}
