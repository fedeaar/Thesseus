#include "renderer.h"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

//
// constructor
//

core::code
render::ImguiRenderer::init(mgmt::vulkan::Swapchain& swapchain)
{
  if (initialized == core::status::INIT) {
    return core::code::SUCCESS;
  }
  if (initialized == core::status::ERROR) {
    return core::code::IN_ERROR_STATE;
  }
  // init context
  ImGui::CreateContext();
  // sdl impl
  if (!ImGui_ImplSDL3_InitForVulkan(window_mgr_->get_window())) {
    core::Logger::err("render::ImguiRenderer::init",
                      "failed to initialize imgui sdl3 impl.");
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
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
  auto pool_result = vk_mgr_->create_descriptor_pool(pool_info);
  if (!pool_result.has_value()) {
    core::Logger::err("render::ImguiRenderer::init",
                      "failed to create descriptor pool.");
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  imgui_pool_ = pool_result.value();
  // vulkan impl
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = vk_mgr_->get_instance();
  init_info.PhysicalDevice = vk_mgr_->get_physical_dev();
  init_info.Device = vk_mgr_->get_dev();
  init_info.Queue = vk_mgr_->get_graphics_queue();
  init_info.DescriptorPool = imgui_pool_;
  init_info.MinImageCount = 3;
  init_info.ImageCount = 3;
  init_info.UseDynamicRendering = true;
  init_info.PipelineRenderingCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO
  };
  init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
  init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats =
    &swapchain.image_fmt;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  if (!ImGui_ImplVulkan_Init(&init_info)) {
    core::Logger::err("render::ImguiRenderer::init",
                      "failed to initialize imgui vulkan impl.");
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // textures
  if (!ImGui_ImplVulkan_CreateFontsTexture()) {
    core::Logger::err("render::ImguiRenderer::init",
                      "failed to create fonts texture.");
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // success
  initialized = core::status::INIT;
  return core::code::SUCCESS;
}

render::ImguiRenderer::ImguiRenderer(mgmt::vulkan::Manager* vk_mgr,
                                     mgmt::window::Manager* window_mgr)
  : render::Renderer{ vk_mgr }
  , window_mgr_{ window_mgr } {};

//
// destructor
//

core::code
render::ImguiRenderer::destroy()
{
  if (initialized == core::status::NOT_INIT) {
    return core::code::SUCCESS;
  }
  vk_mgr_->device_wait_idle();
  ImGui_ImplVulkan_Shutdown();
  // vk_mgr_->destroy_descriptor_pool(imgui_pool_);
  initialized = core::status::NOT_INIT;
  return core::code::SUCCESS;
}

render::ImguiRenderer::~ImguiRenderer()
{
  destroy();
}

//
// draw
//

void
render::ImguiRenderer::draw(mgmt::vulkan::Swapchain& swapchain)
{
  // we assume we are init
  auto cmd = swapchain.get_current_cmd_buffer();
  auto img = swapchain.get_current_image();
  auto img_view = swapchain.get_current_image_view();
  auto color_attachment = mgmt::vulkan::info::color_attachment_info(
    img_view, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  auto renderInfo = mgmt::vulkan::info::rendering_info(
    swapchain.extent, &color_attachment, nullptr);
  mgmt::vulkan::image::transition_image(
    cmd,
    img,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  vkCmdBeginRendering(cmd, &renderInfo);
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
  vkCmdEndRendering(cmd);
}
