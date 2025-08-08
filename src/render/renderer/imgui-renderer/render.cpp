#include "renderer.h"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <numeric>

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
    ERR("failed to initialize imgui sdl3 impl.");
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // create pool
  VkDescriptorPoolSize poolSizes[] = {
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE }
  };
  VkDescriptorPoolCreateInfo poolInfo = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
    .maxSets = 0,
    .poolSizeCount = (uint32_t)IM_ARRAYSIZE(poolSizes),
    .pPoolSizes = poolSizes
  };
  for (VkDescriptorPoolSize& pool_size : poolSizes) {
    poolInfo.maxSets += pool_size.descriptorCount;
  }
  auto poolResult = p_vkMgr_->create_descriptor_pool(poolInfo);
  if (!poolResult.has_value()) {
    core::Logger::err("render::ImguiRenderer::init",
                      "failed to create descriptor pool.");
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  imguiPool_ = poolResult.value();
  // vulkan impl
  ImGui_ImplVulkan_InitInfo
    initInfo = { .Instance = p_vkMgr_->get_instance(),
                 .PhysicalDevice = p_vkMgr_->get_physical_dev(),
                 .Device = p_vkMgr_->get_dev(),
                 .QueueFamily = 0,
                 .Queue = p_vkMgr_->get_graphics_queue(),
                 .DescriptorPool = imguiPool_,
                 .RenderPass = nullptr,
                 .MinImageCount = 3,
                 .ImageCount = 3,
                 .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
                 .Subpass = 0,
                 .UseDynamicRendering = true,
                 .PipelineRenderingCreateInfo = {
                   .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
                   .colorAttachmentCount = 1,
                   .pColorAttachmentFormats = &swapchain.surface_fmt.format,
                  },
                 .MinAllocationSize = 0
                };
  if (!ImGui_ImplVulkan_Init(&initInfo)) {
    ERR("failed to initialize imgui vulkan impl.");
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // textures
  if (!ImGui_ImplVulkan_CreateFontsTexture()) {
    ERR("failed to create imgui fonts texture.");
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // success
  initialized = core::status::INITIALIZED;
  return core::code::SUCCESS;
}

render::ImguiRenderer::ImguiRenderer(mgmt::vulkan::Swapchain* mp_swapchain,
                                     mgmt::vulkan::Manager* mp_vkMgr,
                                     mgmt::window::Manager* mp_windowMgr,
                                     debug::GlobalStats* p_stats)
  : render::Renderer{ mp_vkMgr, mp_swapchain, p_stats }
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
  if (initialized == core::status::ERROR) {
    return core::code::IN_ERROR_STATE;
  }
  p_vkMgr_->device_wait_idle();
  ImGui_ImplVulkan_Shutdown();
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
draw_global_stats(debug::GlobalStats& stats)
{
  ImGui::NewFrame();
  if (ImGui::Begin("Stats")) {
    ImGui::Text("frametime %f ms", stats.frametime);
    ImGui::Text("draw time %f ms", stats.meshDrawTime);
    ImGui::Text("triangles %i", stats.triangleCount);
    ImGui::Text("draws %i", stats.drawcallCount);
  }
  ImGui::End();
  ImGui::Render();
}

void
render::ImguiRenderer::draw()
{
  draw_global_stats(*p_stats_);
  mgmt::vulkan::Swapchain& swapchain = *p_swapchain_;
  // we assume we are init
  auto cmd = swapchain.get_current_cmd_buffer();
  auto img = swapchain.get_current_image();
  auto imgView = swapchain.get_current_image_view();
  auto colorAttachment = mgmt::vulkan::info::color_attachment_info(
    imgView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  auto renderInfo = mgmt::vulkan::info::rendering_info(
    swapchain.extent, &colorAttachment, nullptr);
  mgmt::vulkan::image::transition_image(
    cmd,
    img,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  vkCmdBeginRendering(cmd, &renderInfo);
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
  vkCmdEndRendering(cmd);
}
