#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>

//
// constructor
//

core::code
render::AssetRenderer::init(mgmt::vulkan::swapchain::Swapchain& swapchain)
{
  if (initialized) {
    return core::code::SUCCESS;
  }
  // layout
  mgmt::vulkan::descriptor::LayoutBuilder layout_builder;
  layout_builder.add_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  auto result =
    layout_builder.build(vk_mgr_->get_dev(), VK_SHADER_STAGE_FRAGMENT_BIT);
  if (!result.has_value()) {
    // TODO
    return core::code::ERROR;
  }
  single_img_layout_ = result.value();
  VkPushConstantRange buff_range{};
  buff_range.offset = 0;
  buff_range.size = sizeof(mgmt::vulkan::mesh::GPUDrawPushConstants);
  buff_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  VkPipelineLayoutCreateInfo layout_info =
    mgmt::vulkan::info::pipeline_layout_create_info();
  layout_info.pPushConstantRanges = &buff_range;
  layout_info.pushConstantRangeCount = 1;
  layout_info.pSetLayouts = &single_img_layout_;
  layout_info.setLayoutCount = 1;
  // builder
  mgmt::vulkan::pipeline::Builder builder;
  builder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
  builder.set_polygon_mode(VK_POLYGON_MODE_FILL);
  builder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
  builder.set_multisampling_none();
  // builder.enable_blending_additive();
  builder.disable_blending();
  builder.enable_depthtest(true, VK_COMPARE_OP_LESS_OR_EQUAL);
  builder.set_color_attachment_format(swapchain.draw_img.format);
  builder.set_depth_format(swapchain.depth_img.format);
  // pipeline initc
  auto pipeline_result =
    vk_mgr_->create_gfx_pipeline(layout_info,
                                 builder,
                                 "./shaders/colored_triangle_mesh_vs.vert.spv",
                                 "./shaders/tex_image.frag.spv");
  if (!pipeline_result.has_value()) {
    core::Logger::err("render::AssetRenderer::init",
                      "create_gfx_pipeline failed to create pipeline");
    return core::code::ERROR;
  }
  pipeline_ = pipeline_result.value();
  // init mesh
  auto meshes_result = vk_mgr_->load_gltf_meshes("./res/meshes/basicmesh.glb");
  if (!meshes_result.has_value()) {
    core::Logger::err("render::AssetRenderer::init", "could not load meshes");
    return core::code::ERROR;
  }
  meshes_ = meshes_result.value();
  mgmt::vulkan::descriptor::LayoutBuilder descriptor_builder;
  descriptor_builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  scene_layout_ =
    descriptor_builder
      .build(vk_mgr_->get_dev(),
             VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
      .value(); // TODO: check ok and maybe abstract this
                // default imgs
  u32 white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
  auto img_result = vk_mgr_->create_image((void*)&white,
                                          VkExtent3D{ 1, 1, 1 },
                                          VK_FORMAT_R8G8B8A8_UNORM,
                                          VK_IMAGE_USAGE_SAMPLED_BIT);
  if (!img_result.has_value()) {
    core::Logger::err("render::AssetRenderer::init",
                      "could not load white img");
    return core::code::ERROR;
  }
  white_img_ = img_result.value();
  u32 gray = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
  img_result = vk_mgr_->create_image((void*)&gray,
                                     VkExtent3D{ 1, 1, 1 },
                                     VK_FORMAT_R8G8B8A8_UNORM,
                                     VK_IMAGE_USAGE_SAMPLED_BIT);
  if (!img_result.has_value()) {
    core::Logger::err("render::AssetRenderer::init", "could not load gray img");
    return core::code::ERROR;
  }
  gray_img_ = img_result.value();
  u32 black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 0));
  img_result = vk_mgr_->create_image((void*)&black,
                                     VkExtent3D{ 1, 1, 1 },
                                     VK_FORMAT_R8G8B8A8_UNORM,
                                     VK_IMAGE_USAGE_SAMPLED_BIT);
  if (!img_result.has_value()) {
    core::Logger::err("render::AssetRenderer::init",
                      "could not load black img");
    return core::code::ERROR;
  }
  black_img_ = img_result.value();
  u32 magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
  std::array<u32, 16 * 16> pixels;
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
    }
  }
  img_result = vk_mgr_->create_image(pixels.data(),
                                     VkExtent3D{ 16, 16, 1 },
                                     VK_FORMAT_R8G8B8A8_UNORM,
                                     VK_IMAGE_USAGE_SAMPLED_BIT);
  if (!img_result.has_value()) {
    core::Logger::err("render::AssetRenderer::init",
                      "could not load checker img");
    return core::code::ERROR;
  }
  error_checker_img_ = img_result.value();
  // create default samplers
  VkSamplerCreateInfo sampl = { .sType =
                                  VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
  sampl.magFilter = VK_FILTER_NEAREST;
  sampl.minFilter = VK_FILTER_NEAREST;
  vkCreateSampler(
    vk_mgr_->get_dev(), &sampl, nullptr, &default_nearest_sampler_);

  sampl.magFilter = VK_FILTER_LINEAR;
  sampl.minFilter = VK_FILTER_LINEAR;
  vkCreateSampler(
    vk_mgr_->get_dev(), &sampl, nullptr, &default_linear_sampler_);

  // delete the rectangle data on engine shutdown
  del_queue_.push([=]() {
    vkDestroySampler(vk_mgr_->get_dev(), default_linear_sampler_, nullptr);
    vkDestroySampler(vk_mgr_->get_dev(), default_nearest_sampler_, nullptr);
    vk_mgr_->destroy_image(white_img_);
    vk_mgr_->destroy_image(gray_img_);
    vk_mgr_->destroy_image(black_img_);
    vk_mgr_->destroy_image(error_checker_img_);
    vkDestroyDescriptorSetLayout(vk_mgr_->get_dev(), scene_layout_, nullptr);
    vkDestroyDescriptorSetLayout(
      vk_mgr_->get_dev(), single_img_layout_, nullptr);
    for (auto& mesh : meshes_) {
      vk_mgr_->destroy_buffer(mesh->mesh_buffers.index_buff);
      vk_mgr_->destroy_buffer(mesh->mesh_buffers.vertex_buff);
    }
  });
  // success
  initialized = true;
  return core::code::SUCCESS;
}

render::AssetRenderer::AssetRenderer(mgmt::vulkan::Manager* vk_mgr)
  : render::Renderer{ vk_mgr } {};

//
// destructor
//

core::code
render::AssetRenderer::destroy()
{
  // todo@engine: handle pipes?
  del_queue_.flush();
  initialized = false;
  return core::code::SUCCESS;
}

render::AssetRenderer::~AssetRenderer()
{
  if (initialized) {
    destroy();
  }
}

//
// draw
//

void
render::AssetRenderer::draw(mgmt::vulkan::swapchain::Swapchain& swapchain,
                            Camera& camera)
{
  auto cmd = swapchain.get_current_cmd_buffer();
  auto current_frame = swapchain.get_current_frame();
  // mgmt::vulkan::buffer::AllocatedBuffer scene_buffer =
  //   vk_mgr_
  //     ->create_buffer(sizeof(GPUSceneData),
  //                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
  //                     VMA_MEMORY_USAGE_CPU_TO_GPU)
  //     .value(); // TODO
  // current_frame.del_queue.push(
  //   [=, this]() { vk_mgr_->destroy_buffer(scene_buffer); });
  // void* data;
  // vmaMapMemory(vk_mgr_->get_allocator(), scene_buffer.allocation, &data);
  // memcpy(data, &scene_, sizeof(scene_));
  // vmaUnmapMemory(vk_mgr_->get_allocator(), scene_buffer.allocation);
  // // create a descriptor set that binds that buffer and update it
  // VkDescriptorSet global_descriptor_set =
  //   current_frame.frame_descriptors.allocate(vk_mgr_->get_dev(),
  //   scene_layout_);
  // mgmt::vulkan::descriptor::Writer writer;
  // writer.write_buffer(0,
  //                     scene_buffer.buffer,
  //                     sizeof(GPUSceneData),
  //                     0,
  //                     VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  // writer.update_set(vk_mgr_->get_dev(), global_descriptor_set);
  auto color_attachment = mgmt::vulkan::info::color_attachment_info(
    swapchain.draw_img.view, nullptr, VK_IMAGE_LAYOUT_GENERAL);
  auto depth_attachment = mgmt::vulkan::info::depth_attachment_info(
    swapchain.depth_img.view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
  auto render_info = mgmt::vulkan::info::rendering_info(
    swapchain.draw_extent, &color_attachment, &depth_attachment);
  vkCmdBeginRendering(cmd, &render_info);
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_.pipeline);
  VkDescriptorSet set = current_frame.frame_descriptors.allocate(
    vk_mgr_->get_dev(), single_img_layout_);
  mgmt::vulkan::descriptor::Writer single_img_writer;
  single_img_writer.write_image(0,
                                error_checker_img_.view,
                                default_nearest_sampler_,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  single_img_writer.update_set(vk_mgr_->get_dev(), set);
  vkCmdBindDescriptorSets(cmd,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline_.layout,
                          0,
                          1,
                          &set,
                          0,
                          nullptr);
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
  mgmt::vulkan::mesh::GPUDrawPushConstants push_constants;
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
};
