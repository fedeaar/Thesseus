#include "renderer.h"

//
// constructor
//

core::Status
render::TriangleRenderer::init(mgmt::vulkan::swapchain::Swapchain& swapchain)
{
  if (initialized) {
    return core::Status::SUCCESS;
  }
  swapchain_ = swapchain;
  // layout
  VkPipelineLayoutCreateInfo layout_info =
    mgmt::vulkan::info::pipeline_layout_create_info();
  // builder
  mgmt::vulkan::pipeline::Builder builder;
  builder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
  builder.set_polygon_mode(VK_POLYGON_MODE_FILL);
  builder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
  builder.set_multisampling_none();
  builder.disable_blending();
  builder.disable_depthtest();
  builder.set_color_attachment_format(swapchain_.draw_img.format);
  builder.set_depth_format(VK_FORMAT_UNDEFINED);
  // pipeline init
  auto pipeline_result =
    vk_mgr_->create_gfx_pipeline(layout_info,
                                 builder,
                                 "./shaders/colored_triangle_vs.vert.spv",
                                 "./shaders/colored_triangle_fs.frag.spv");
  if (!pipeline_result.has_value()) {
    logger_.err("init failed, create_gfx_pipeline failed to create pipeline");
    return core::Status::ERROR;
  }
  pipeline_ = pipeline_result.value();
  // success
  initialized = true;
  return core::Status::SUCCESS;
}

render::TriangleRenderer::TriangleRenderer(mgmt::vulkan::Manager* vk_mgr)
  : render::Renderer{ vk_mgr } {};

//
// destructor
//

core::Status
render::TriangleRenderer::destroy()
{
  // todo@engine: handle pipes?
  initialized = false;
  return core::Status::SUCCESS;
}

render::TriangleRenderer::~TriangleRenderer()
{
  if (initialized) {
    destroy();
  }
}

//
// draw
//

core::Status
render::TriangleRenderer::draw(VkCommandBuffer cmd,
                               u32 img_idx,
                               mgmt::vulkan::swapchain::Swapchain& swapchain)
{
  VkRenderingAttachmentInfo color_attachment =
    mgmt::vulkan::info::rendering_attachment_info(
      swapchain.draw_img.view,
      nullptr,
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  VkRenderingInfo render_info = mgmt::vulkan::info::rendering_info(
    swapchain_.draw_extent, &color_attachment, nullptr);
  vkCmdBeginRendering(cmd, &render_info);
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_.pipeline);
  VkViewport viewport = {};
  viewport.x = 0;
  viewport.y = 0;
  viewport.width = swapchain_.draw_extent.width;
  viewport.height = swapchain_.draw_extent.height;
  viewport.minDepth = 0.f;
  viewport.maxDepth = 1.f;
  vkCmdSetViewport(cmd, 0, 1, &viewport);
  VkRect2D scissor = {};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent.width = swapchain_.draw_extent.width;
  scissor.extent.height = swapchain_.draw_extent.height;
  vkCmdSetScissor(cmd, 0, 1, &scissor);
  vkCmdDraw(cmd, 3, 1, 0, 0);
  vkCmdEndRendering(cmd);
  return core::Status::SUCCESS;
};
