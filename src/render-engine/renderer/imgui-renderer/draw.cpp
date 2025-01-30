#include "imgui-renderer.h"

RenderEngine::Status
RenderEngine::ImguiRenderer::draw(
  VkCommandBuffer cmd,
  u32 img_idx,
  ResourceManagement::VulkanManager::Swapchain::Swapchain& swapchain)
{
  auto img = swapchain.imgs[img_idx];
  auto img_view = swapchain.img_views[img_idx];
  ResourceManagement::VulkanManager::Image::transition_image(
    cmd,
    img,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  VkRenderingAttachmentInfo color_attachment =
    ResourceManagement::VulkanManager::Info::rendering_attachment_info(
      img_view, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  VkRenderingInfo renderInfo =
    ResourceManagement::VulkanManager::Info::rendering_info(
      swapchain.extent, &color_attachment, nullptr);
  vkCmdBeginRendering(cmd, &renderInfo);
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
  vkCmdEndRendering(cmd);
  ResourceManagement::VulkanManager::Image::transition_image(
    cmd,
    img,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  return RenderEngine::Status::SUCCESS;
}
