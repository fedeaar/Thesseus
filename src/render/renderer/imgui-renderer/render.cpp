#include "renderer.h"

//
// constructor
//

core::Status
render::ImguiRenderer::init(mgmt::vulkan::swapchain::Swapchain& swapchain)
{
  swapchain_ = swapchain;
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
  VkDescriptorPool pool = vk_mgr_->create_descriptor_pool(pool_info).value();
  // init imgui and sdl
  ImGui::CreateContext();
  ImGui_ImplSDL3_InitForVulkan(window_mgr_->get_window());
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = vk_mgr_->get_instance();
  init_info.PhysicalDevice = vk_mgr_->get_physical_dev();
  init_info.Device = vk_mgr_->get_dev();
  init_info.Queue = vk_mgr_->get_graphics_queue();
  init_info.DescriptorPool = pool;
  init_info.MinImageCount = 3;
  init_info.ImageCount = 3;
  init_info.UseDynamicRendering = true;
  init_info.PipelineRenderingCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO
  };
  init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
  init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats =
    &swapchain_.image_fmt;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  ImGui_ImplVulkan_Init(&init_info);
  ImGui_ImplVulkan_CreateFontsTexture();
  // success
  initialized = true;
  return core::Status::SUCCESS;
}

render::ImguiRenderer::ImguiRenderer(mgmt::vulkan::Manager* vk_mgr,
                                     mgmt::WindowManager* window_mgr)
  : render::Renderer{ vk_mgr }
  , window_mgr_{ window_mgr } {};

//
// destructor
//

core::Status
render::ImguiRenderer::destroy()
{
  if (!initialized) {
    return core::Status::SUCCESS;
  }
  vkDeviceWaitIdle(vk_mgr_->get_dev());
  ImGui_ImplVulkan_Shutdown();
  initialized = false;
  return core::Status::SUCCESS;
}

render::ImguiRenderer::~ImguiRenderer()
{
  destroy();
}

//
// draw
//

core::Status
render::ImguiRenderer::draw(VkCommandBuffer cmd,
                            u32 img_idx,
                            mgmt::vulkan::swapchain::Swapchain& swapchain)
{
  ImGui::Render();
  auto img = swapchain.imgs[img_idx];
  auto img_view = swapchain.img_views[img_idx];
  mgmt::vulkan::image::transition_image(
    cmd,
    img,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  VkRenderingAttachmentInfo color_attachment =
    mgmt::vulkan::info::rendering_attachment_info(
      img_view, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  VkRenderingInfo renderInfo = mgmt::vulkan::info::rendering_info(
    swapchain.extent, &color_attachment, nullptr);
  vkCmdBeginRendering(cmd, &renderInfo);
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
  vkCmdEndRendering(cmd);
  return core::Status::SUCCESS;
}
