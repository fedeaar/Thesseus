#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>

//
// constructor
//

core::Status
render::CustomMeshRenderer::init(mgmt::vulkan::swapchain::Swapchain& swapchain)
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
  builder.enable_blending_additive();
  builder.enable_depthtest(true, VK_COMPARE_OP_LESS_OR_EQUAL);
  builder.set_color_attachment_format(swapchain_.draw_img.format);
  builder.set_depth_format(swapchain_.depth_img.format);
  // pipeline initc
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
  auto meshes_result = vk_mgr_->load_gltf_meshes("./res/basicmesh.glb");
  if (!meshes_result.has_value()) {
    logger_.err("could not load meshes");
    return core::Status::ERROR;
  }
  meshes_ = meshes_result.value();
  // delete the rectangle data on engine shutdown
  del_queue_.push([=]() {
    for (auto& mesh : meshes_) {
      vk_mgr_->destroy_buffer(mesh->mesh_buffers.index_buff);
      vk_mgr_->destroy_buffer(mesh->mesh_buffers.vertex_buff);
    }
  });
  // success
  initialized = true;
  return core::Status::SUCCESS;
}

render::CustomMeshRenderer::CustomMeshRenderer(mgmt::vulkan::Manager* vk_mgr)
  : render::Renderer{ vk_mgr } {};

//
// destructor
//

core::Status
render::CustomMeshRenderer::destroy()
{
  // todo@engine: handle pipes?
  del_queue_.flush();
  initialized = false;
  return core::Status::SUCCESS;
}

render::CustomMeshRenderer::~CustomMeshRenderer()
{
  if (initialized) {
    destroy();
  }
}

//
// draw
//

core::Status
render::CustomMeshRenderer::draw(VkCommandBuffer cmd,
                                 u32 img_idx,
                                 mgmt::vulkan::swapchain::Swapchain& swapchain,
                                 Camera& camera)
{
  VkRenderingAttachmentInfo color_attachment =
    mgmt::vulkan::info::color_attachment_info(
      swapchain.draw_img.view, nullptr, VK_IMAGE_LAYOUT_GENERAL);
  VkRenderingAttachmentInfo depth_attachment =
    mgmt::vulkan::info::depth_attachment_info(
      swapchain_.depth_img.view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
  VkRenderingInfo render_info = mgmt::vulkan::info::rendering_info(
    swapchain_.draw_extent, &color_attachment, &depth_attachment);
  vkCmdBeginRendering(cmd, &render_info);
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_.pipeline);
  mgmt::vulkan::mesh::GPUDrawPushConstants push_constants;
  VkViewport viewport = {};
  viewport.x = 0;
  viewport.y = 0;
  viewport.width = swapchain.draw_extent.width;
  viewport.height = swapchain.draw_extent.height;
  viewport.minDepth = 0.f;
  viewport.maxDepth = 1.f;
  vkCmdSetViewport(cmd, 0, 1, &viewport);
  VkRect2D scissor = {};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent.width = viewport.width;
  scissor.extent.height = viewport.height;
  vkCmdSetScissor(cmd, 0, 1, &scissor);
  push_constants.vertex_buff_addr = meshes_[2]->mesh_buffers.vertex_buff_addr;
  m4f view = camera.view_matrix();
  m4f projection = camera.proj_matrix();
  push_constants.world_matrix = projection * view;
  vkCmdPushConstants(cmd,
                     pipeline_.layout,
                     VK_SHADER_STAGE_VERTEX_BIT,
                     0,
                     sizeof(mgmt::vulkan::mesh::GPUDrawPushConstants),
                     &push_constants);
  vkCmdBindIndexBuffer(
    cmd, meshes_[2]->mesh_buffers.index_buff.buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(cmd,
                   meshes_[2]->surfaces[0].count,
                   1,
                   meshes_[2]->surfaces[0].start_idx,
                   0,
                   0);
  vkCmdEndRendering(cmd);
  return core::Status::SUCCESS;
};
