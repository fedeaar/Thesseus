#include "renderer.h"

//
// constructor
//

core::code
render::GLTF_MetallicRoughnessRenderer::init(
  mgmt::vulkan::Swapchain& swapchain,
  render::AssetRenderer& asset_renderer)
{
  if (initialized) {
    return core::code::SUCCESS;
  }
  VkDevice dev = vk_mgr_->get_dev();
  auto vs_result =
    mgmt::vulkan::pipeline::load_shader_module("./shaders/mesh.vert.spv", dev);
  if (!vs_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::create_gfx_pipeline",
                      "error when building the vertex shader");
    return core::code::ERROR;
  }
  auto vs = vs_result.value();
  auto fs_result =
    mgmt::vulkan::pipeline::load_shader_module("./shaders/mesh.frag.spv", dev);
  if (!fs_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::create_gfx_pipeline",
                      "error when building the fragment shader");
    return core::code::ERROR;
  }
  auto fs = fs_result.value();
  // layout
  VkPushConstantRange matrix_range{};
  matrix_range.offset = 0;
  matrix_range.size = sizeof(mgmt::vulkan::mesh::GPUDrawPushConstants);
  matrix_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  mgmt::vulkan::descriptor::LayoutBuilder layout_builder;
  layout_builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  layout_builder.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  layout_builder.add_binding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  material_layout_ =
    layout_builder
      .build(dev, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
      .value(); // TODO: check ok and maybe abstract this
  scene_layout_ = asset_renderer.scene_layout_;
  VkDescriptorSetLayout layouts[] = { scene_layout_, material_layout_ };
  VkPipelineLayoutCreateInfo mesh_layout_info =
    mgmt::vulkan::info::pipeline_layout_create_info();
  mesh_layout_info.setLayoutCount = 2;
  mesh_layout_info.pSetLayouts = layouts;
  mesh_layout_info.pPushConstantRanges = &matrix_range;
  mesh_layout_info.pushConstantRangeCount = 1;
  // builder
  VkPipelineLayout pipeline_layout;
  auto status = mgmt::vulkan::check(
    vkCreatePipelineLayout(dev, &mesh_layout_info, nullptr, &pipeline_layout));
  if (status != core::code::SUCCESS) {
    core::Logger::err("render::GLTF_MetallicRoughnessRenderer::init",
                      "error when building the pipeline layout");
    return core::code::ERROR;
  }
  opaque_pipe_.layout = pipeline_layout;
  transparent_pipe_.layout = pipeline_layout;
  mgmt::vulkan::pipeline::Builder builder;
  builder.set_shaders(vs, fs);
  builder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
  builder.set_polygon_mode(VK_POLYGON_MODE_FILL);
  builder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
  builder.set_multisampling_none();
  builder.disable_blending();
  builder.enable_depthtest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);
  // render format
  builder.set_color_attachment_format(swapchain.draw_img.format);
  builder.set_depth_format(swapchain.depth_img.format);
  // use the triangle layout we created
  builder.set_layout(pipeline_layout);
  // finally build the pipeline
  opaque_pipe_.pipe = builder.build_pipeline(dev).value(); // TODO
  // create the transparent variant
  builder.enable_blending_additive();
  builder.enable_depthtest(false, VK_COMPARE_OP_GREATER_OR_EQUAL);
  transparent_pipe_.pipe = builder.build_pipeline(dev).value(); // TODO
  vkDestroyShaderModule(dev, fs, nullptr);
  vkDestroyShaderModule(dev, vs, nullptr);
  MaterialResources res;
  // default the material textures
  res.colorImage = asset_renderer.white_img_;
  res.colorSampler = asset_renderer.default_linear_sampler_;
  res.metalRoughImage = asset_renderer.white_img_;
  res.metalRoughSampler = asset_renderer.default_linear_sampler_;
  mgmt::vulkan::buffer::AllocatedBuffer constants =
    vk_mgr_
      ->create_buffer(sizeof(MaterialConstants),
                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VMA_MEMORY_USAGE_CPU_TO_GPU)
      .value(); // TODO
  MaterialConstants* scene_uniform_data;
  vmaMapMemory(vk_mgr_->get_allocator(),
               constants.allocation,
               (void**)&scene_uniform_data);
  memcpy(scene_uniform_data, &scene_, sizeof(scene_));
  vmaUnmapMemory(vk_mgr_->get_allocator(), constants.allocation);
  scene_uniform_data->color_factors = glm::vec4{ 1, 1, 1, 1 };
  scene_uniform_data->metal_rough_factors = glm::vec4{ 1, 0.5, 0, 0 };
  del_queue_.push([=, this]() { vk_mgr_->destroy_buffer(constants); });
  res.dataBuffer = constants.buffer;
  res.dataBufferOffset = 0;
  default_data_ = write_material(mgmt::vulkan::material::Type::opaque,
                                 res,
                                 vk_mgr_->get_global_descriptor_allocator());
  for (auto& m : asset_renderer.meshes_) {
    std::shared_ptr<MeshNode> new_node = std::make_shared<MeshNode>();
    new_node->mesh = m;
    new_node->local_tf = glm::mat4{ 1.f };
    new_node->world_tf = glm::mat4{ 1.f };
    for (auto& s : new_node->mesh->surfaces) {
      s.material =
        std::make_shared<mgmt::vulkan::material::Material>(default_data_);
    }
    loaded_nodes_[m->name] = std::move(new_node);
  }

  // success
  initialized = true;
  return core::code::SUCCESS;
}

render::GLTF_MetallicRoughnessRenderer::GLTF_MetallicRoughnessRenderer(
  mgmt::vulkan::Manager* vk_mgr)
  : render::Renderer{ vk_mgr } {};

//
// destructor
//

core::code
render::GLTF_MetallicRoughnessRenderer::destroy()
{
  del_queue_.flush();
  initialized = false;
  return core::code::SUCCESS;
}

render::GLTF_MetallicRoughnessRenderer::~GLTF_MetallicRoughnessRenderer()
{
  if (initialized) {
    destroy();
  }
}

//
// write material
//

mgmt::vulkan::material::Instance
render::GLTF_MetallicRoughnessRenderer::write_material(
  mgmt::vulkan::material::Type pass,
  const MaterialResources& resources,
  mgmt::vulkan::descriptor::DynamicAllocator& descriptor_allocator)
{
  auto dev = vk_mgr_->get_dev();
  mgmt::vulkan::material::Instance data;
  data.type = pass;
  if (pass == mgmt::vulkan::material::Type::transparent) {
    data.pipe = &transparent_pipe_;
  } else {
    data.pipe = &opaque_pipe_;
  }
  data.material_set = descriptor_allocator.allocate(dev, material_layout_);
  writer_.clear();
  writer_.write_buffer(0,
                       resources.dataBuffer,
                       sizeof(MaterialConstants),
                       resources.dataBufferOffset,
                       VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  writer_.write_image(1,
                      resources.colorImage.view,
                      resources.colorSampler,
                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  writer_.write_image(2,
                      resources.metalRoughImage.view,
                      resources.metalRoughSampler,
                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  writer_.update_set(dev, data.material_set);
  return data;
}

//
// draw
//

void
render::GLTF_MetallicRoughnessRenderer::draw(mgmt::vulkan::Swapchain& swapchain,
                                             Camera& camera)
{
  auto cmd = swapchain.get_current_cmd_buffer();
  auto current_frame = swapchain.get_current_frame();
  auto color_attachment = mgmt::vulkan::info::color_attachment_info(
    swapchain.draw_img.view, nullptr, VK_IMAGE_LAYOUT_GENERAL);
  auto depth_attachment = mgmt::vulkan::info::depth_attachment_info(
    swapchain.depth_img.view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
  auto render_info = mgmt::vulkan::info::rendering_info(
    swapchain.draw_extent, &color_attachment, &depth_attachment);
  vkCmdBeginRendering(cmd, &render_info);
  mgmt::vulkan::buffer::AllocatedBuffer scene_buffer =
    vk_mgr_
      ->create_buffer(sizeof(GPUSceneData),
                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VMA_MEMORY_USAGE_CPU_TO_GPU)
      .value(); // TODO
  current_frame.del_queue.push(
    [=, this]() { vk_mgr_->destroy_buffer(scene_buffer); });
  void* data;
  vmaMapMemory(vk_mgr_->get_allocator(), scene_buffer.allocation, &data);
  memcpy(data, &scene_, sizeof(scene_));
  vmaUnmapMemory(vk_mgr_->get_allocator(), scene_buffer.allocation);
  // create a descriptor set that binds that buffer and update it
  VkDescriptorSet global_descriptor_set =
    current_frame.frame_descriptors.allocate(vk_mgr_->get_dev(), scene_layout_);
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
  mgmt::vulkan::descriptor::Writer writer;
  writer.write_buffer(0,
                      scene_buffer.buffer,
                      sizeof(GPUSceneData),
                      0,
                      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  writer.update_set(vk_mgr_->get_dev(), global_descriptor_set);
  mgmt::vulkan::mesh::GPUDrawPushConstants push_constants;
  m4f view = camera.view_matrix();
  m4f projection = camera.proj_matrix();
  push_constants.world_matrix = projection * view;
  for (const render::Object& draw : main_draw_ctx_.opaque_surfaces) {
    vkCmdBindPipeline(
      cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.material->pipe->pipe);
    vkCmdBindDescriptorSets(cmd,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            draw.material->pipe->layout,
                            0,
                            1,
                            &global_descriptor_set,
                            0,
                            nullptr);
    vkCmdBindDescriptorSets(cmd,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            draw.material->pipe->layout,
                            1,
                            1,
                            &draw.material->material_set,
                            0,
                            nullptr);
    push_constants.vertex_buff_addr = draw.vertex_buff_addr;
    vkCmdPushConstants(cmd,
                       draw.material->pipe->layout,
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(mgmt::vulkan::mesh::GPUDrawPushConstants),
                       &push_constants);
    vkCmdBindIndexBuffer(cmd, draw.idx_buff, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmd, draw.idx_count, 1, draw.start_idx, 0, 0);
  }
  vkCmdEndRendering(cmd);
};

void
render::GLTF_MetallicRoughnessRenderer::update_scene()
{
  main_draw_ctx_.opaque_surfaces.clear();
  loaded_nodes_["Suzanne"]->Draw(glm::mat4{ 5.f }, main_draw_ctx_);
  for (int x = -3; x < 3; x++) {
    glm::mat4 transform = m4f(1.0f);
    glm::mat4 scale = glm::scale(transform, glm::vec3{ 0.2 });
    glm::mat4 translation = glm::translate(transform, glm::vec3{ x, 1, 0 });
    loaded_nodes_["Cube"]->Draw(translation * scale, main_draw_ctx_);
  }

  // some default lighting parameters
  scene_.ambient = glm::vec4(.1f);
  scene_.sun_color = glm::vec4(1.f);
  scene_.sun_dir = glm::vec4(0, 1, 0.5, 1.f);
}
