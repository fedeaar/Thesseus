#include "renderer.h"

//
// constructor
//

core::Status
render::TriangleMeshRenderer::init(
  mgmt::vulkan::swapchain::Swapchain& swapchain)
{
  if (initialized) {
    return core::Status::SUCCESS;
  }
  swapchain_ = swapchain;
  // layout
  VkPushConstantRange buff_range{};
  buff_range.offset = 0;
  buff_range.size = sizeof(mgmt::vulkan::mesh::GPUDrawPushConstants);
  buff_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  VkPipelineLayoutCreateInfo layout_info =
    mgmt::vulkan::info::pipeline_layout_create_info();
  layout_info.pPushConstantRanges = &buff_range;
  layout_info.pushConstantRangeCount = 1;
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
                                 "./shaders/colored_triangle_mesh_vs.vert.spv",
                                 "./shaders/colored_triangle_mesh_fs.frag.spv");
  if (!pipeline_result.has_value()) {
    logger_.err("init failed, create_gfx_pipeline failed to create pipeline");
    return core::Status::ERROR;
  }
  pipeline_ = pipeline_result.value();
  // init mesh
  std::array<mgmt::vulkan::mesh::Vertex, 4> rect_vertices;
  rect_vertices[0].position = { 0.5, -0.5, 0 };
  rect_vertices[1].position = { 0.5, 0.5, 0 };
  rect_vertices[2].position = { -0.5, -0.5, 0 };
  rect_vertices[3].position = { -0.5, 0.5, 0 };
  rect_vertices[0].color = { 0, 0, 0, 1 };
  rect_vertices[1].color = { 0.5, 0.5, 0.5, 1 };
  rect_vertices[2].color = { 1, 0, 0, 1 };
  rect_vertices[3].color = { 0, 1, 0, 1 };
  std::array<u32, 6> rect_indices;
  rect_indices[0] = 0;
  rect_indices[1] = 1;
  rect_indices[2] = 2;
  rect_indices[3] = 2;
  rect_indices[4] = 1;
  rect_indices[5] = 3;
  auto rectangle_result = vk_mgr_->upload_mesh(rect_indices, rect_vertices);
  if (!rectangle_result.has_value()) {
    logger_.err("init failed, could not to upload mesh");
    return core::Status::ERROR;
  }
  rectangle_ = rectangle_result.value();
  // delete the rectangle data on engine shutdown
  del_queue_.push([=]() {
    vk_mgr_->destroy_buffer(rectangle_.index_buff);
    vk_mgr_->destroy_buffer(rectangle_.vertex_buff);
  });
  // success
  initialized = true;
  return core::Status::SUCCESS;
}

render::TriangleMeshRenderer::TriangleMeshRenderer(
  mgmt::vulkan::Manager* vk_mgr)
  : render::Renderer{ vk_mgr } {};

//
// destructor
//

core::Status
render::TriangleMeshRenderer::destroy()
{
  // todo@engine: handle pipes?
  del_queue_.flush();
  initialized = false;
  return core::Status::SUCCESS;
}

render::TriangleMeshRenderer::~TriangleMeshRenderer()
{
  if (initialized) {
    destroy();
  }
}

//
// draw
//

core::Status
render::TriangleMeshRenderer::draw(
  VkCommandBuffer cmd,
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
  vkCmdDraw(cmd, 3, 1, 0, 0);
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_.pipeline);
  mgmt::vulkan::mesh::GPUDrawPushConstants push_constants;
  push_constants.world_matrix = glm::mat4{ 1.f };
  push_constants.vertex_buff_addr = rectangle_.vertex_buff_addr;
  vkCmdPushConstants(cmd,
                     pipeline_.layout,
                     VK_SHADER_STAGE_VERTEX_BIT,
                     0,
                     sizeof(mgmt::vulkan::mesh::GPUDrawPushConstants),
                     &push_constants);
  vkCmdBindIndexBuffer(
    cmd, rectangle_.index_buff.buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
  vkCmdEndRendering(cmd);
  return core::Status::SUCCESS;
};
