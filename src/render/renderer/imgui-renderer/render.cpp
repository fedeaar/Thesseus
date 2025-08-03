#include "renderer.h"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

//
// constructor
//

core::code
render::ImguiRenderer::init()
{
  if (initialized == core::status::INITIALIZED) {
    return core::code::SUCCESS;
  }
  if (initialized == core::status::ERROR) {
    return core::code::IN_ERROR_STATE;
  }
  mgmt::vulkan::Swapchain& swapchain = *p_swapchain_;
  // init context
  ImGui::CreateContext();
  // sdl impl
  if (!ImGui_ImplSDL3_InitForVulkan(p_windowMgr_->get_window())) {
    core::Logger::err("render::ImguiRenderer::init",
                      "failed to initialize imgui sdl3 impl.");
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // create pool
  VkDescriptorPoolSize poolSizes[] = {
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE }
  };
  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  poolInfo.maxSets = 0;
  for (VkDescriptorPoolSize& pool_size : poolSizes) {
    poolInfo.maxSets += pool_size.descriptorCount;
  }
  poolInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(poolSizes);
  poolInfo.pPoolSizes = poolSizes;
  auto poolResult = p_vkMgr_->create_descriptor_pool(poolInfo);
  if (!poolResult.has_value()) {
    core::Logger::err("render::ImguiRenderer::init",
                      "failed to create descriptor pool.");
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  imguiPool_ = poolResult.value();
  // vulkan impl
  ImGui_ImplVulkan_InitInfo initInfo = {};
  initInfo.Instance = p_vkMgr_->get_instance();
  initInfo.PhysicalDevice = p_vkMgr_->get_physical_dev();
  initInfo.Device = p_vkMgr_->get_dev();
  initInfo.Queue = p_vkMgr_->get_graphics_queue();
  initInfo.DescriptorPool = imguiPool_;
  initInfo.MinImageCount = 3;
  initInfo.ImageCount = 3;
  initInfo.UseDynamicRendering = true;
  initInfo.PipelineRenderingCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO
  };
  initInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
  initInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats =
    &swapchain.surface_fmt.format;
  initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  if (!ImGui_ImplVulkan_Init(&initInfo)) {
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
  initialized = core::status::INITIALIZED;
  return core::code::SUCCESS;
}

render::ImguiRenderer::ImguiRenderer(mgmt::vulkan::Swapchain* mp_swapchain,
                                     mgmt::vulkan::Manager* mp_vkMgr,
                                     mgmt::window::Manager* mp_windowMgr)
  : render::Renderer{ mp_vkMgr, mp_swapchain }
  , p_windowMgr_{ mp_windowMgr } {};

//
// destructor
//

core::code
render::ImguiRenderer::destroy()
{
  if (initialized == core::status::NOT_INITIALIZED) {
    return core::code::SUCCESS;
  }
  p_vkMgr_->device_wait_idle();
  ImGui_ImplVulkan_Shutdown();
  // vk_mgr_->destroy_descriptor_pool(imgui_pool_);
  initialized = core::status::NOT_INITIALIZED;
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
render::ImguiRenderer::draw()
{
  mgmt::vulkan::Swapchain& swapchain = *p_swapchain_;
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
