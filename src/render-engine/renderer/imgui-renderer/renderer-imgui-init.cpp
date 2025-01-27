#include "renderer-imgui.h"

using namespace RenderEngine;

ImguiRenderer::ResultStatus
ImguiRenderer::init()
{
  // create pool
  VkDescriptorPoolSize pool_sizes[] = {
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE }
  };
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 0;
  for (VkDescriptorPoolSize& pool_size : pool_sizes) {
    pool_info.maxSets += pool_size.descriptorCount;
  }
  pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;
  VkDescriptorPool pool = manager_->create_descriptor_pool(pool_info).value();
  // init imgui and sdl
  ImGui::CreateContext();
  ImGui_ImplSDL3_InitForVulkan(engine_->window_mgr_.window_);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = manager_->instance_;
  init_info.PhysicalDevice = manager_->gpu_;
  init_info.Device = manager_->device_;
  init_info.Queue = manager_->graphics_queue_;
  init_info.DescriptorPool = pool;
  init_info.MinImageCount = 3;
  init_info.ImageCount = 3;
  init_info.UseDynamicRendering = true;
  init_info.PipelineRenderingCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO
  };
  init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
  init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats =
    &engine_->swapchain_.params_.swapchain_image_fmt_;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  ImGui_ImplVulkan_Init(&init_info);
  ImGui_ImplVulkan_CreateFontsTexture();
  // add deletors
  engine_->del_queue_.push([=]() { // todo@engine: how should we handle this?
    ImGui_ImplVulkan_Shutdown();
  });
  return ResultStatus::SUCCESS;
}
