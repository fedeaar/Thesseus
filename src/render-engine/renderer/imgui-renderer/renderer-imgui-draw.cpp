#include "renderer-imgui.h"

using namespace RenderEngine;

ImguiRenderer::ResultStatus
ImguiRenderer::draw(VkCommandBuffer cmd, VkImageView target)
{
  VkRenderingAttachmentInfo color_attachment = vkinit::attachment_info(
    target, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  VkRenderingInfo renderInfo = vkinit::rendering_info(
    engine_->swapchain_.params_.swapchain_extent_, &color_attachment, nullptr);
  vkCmdBeginRendering(cmd, &renderInfo);
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
  vkCmdEndRendering(cmd);
  return ResultStatus::SUCCESS;
}
