#include "manager.h"

core::Result<mgmt::vulkan::pipeline::Pipeline, core::Status>
mgmt::vulkan::Manager::create_compute_pipeline(
  Swapchain::Swapchain& swapchain,
  VkPipelineLayoutCreateInfo& layout_info,
  char* shader_path)
{
  if (!initialized) {
    logger.err("create_compute_pipeline failed, Manager not initialized");
    return core::Status::NOT_INIT;
  }
  pipeline::Pipeline pipeline;
  // create layout
  auto status = check(
    vkCreatePipelineLayout(device_, &layout_info, nullptr, &pipeline.layout));
  auto load_shader_result = pipeline::load_shader_module(shader_path, device_);
  if (!load_shader_result.has_value()) {
    logger.err(
      "create_compute_pipeline failed, error when building the compute shader");
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
    logger.err(
      "create_compute_pipeline failed, error when creating compute pipeline");
    return status;
  }
  // set destroyer
  vkDestroyShaderModule(device_, shader, nullptr);
  del_queue_.push(
    [=]() { // TODO@engine: no need to keep pipeline existing here ?
      vkDeviceWaitIdle(device_);
      vkDestroyPipelineLayout(device_, pipeline.layout, nullptr);
      vkDestroyPipeline(device_, pipeline.pipeline, nullptr);
    });
  return pipeline;
}

core::Result<mgmt::vulkan::pipeline::Pipeline, core::Status>
mgmt::vulkan::Manager::create_gfx_pipeline(
  VkPipelineLayoutCreateInfo& layout_info,
  pipeline::Builder builder,
  char* vs_path,
  char* fs_path)
{
  if (!initialized) {
    logger.err("create_gfx_pipeline failed, Manager not initialized");
    return core::Status::NOT_INIT;
  }
  pipeline::Pipeline pipeline;
  auto vs_result = mgmt::vulkan::pipeline::load_shader_module(vs_path, device_);
  if (!vs_result.has_value()) {
    logger.err(
      "create_gfx_pipeline failed, error when building the vertex shader");
    return core::Status::ERROR;
  }
  auto vs = vs_result.value();
  auto fs_result = mgmt::vulkan::pipeline::load_shader_module(fs_path, device_);
  if (!fs_result.has_value()) {
    logger.err(
      "create_gfx_pipeline failed, error when building the fragment shader");
    return core::Status::ERROR;
  }
  auto fs = fs_result.value();
  auto status = check(
    vkCreatePipelineLayout(device_, &layout_info, nullptr, &pipeline.layout));
  if (status != core::Status::SUCCESS) {
    logger.err(
      "create_gfx_pipeline failed, error when building the pipeline layout");
    return core::Status::ERROR;
  }
  builder.set_layout(pipeline.layout);
  builder.set_shaders(vs, fs);
  auto pipeline_result = builder.build_pipeline(device_);
  if (!pipeline_result.has_value()) {
    logger.err("create_gfx_pipeline failed, error when building the pipeline");
    return core::Status::ERROR;
  }
  pipeline.pipeline = pipeline_result.value();
  // set destroyer
  vkDestroyShaderModule(device_, fs, nullptr);
  vkDestroyShaderModule(device_, vs, nullptr);
  del_queue_.push(
    [=]() { // TODO@engine: no need to keep pipeline existing here ?
      vkDeviceWaitIdle(device_);
      vkDestroyPipelineLayout(device_, pipeline.layout, nullptr);
      vkDestroyPipeline(device_, pipeline.pipeline, nullptr);
    });
  return pipeline;
}
