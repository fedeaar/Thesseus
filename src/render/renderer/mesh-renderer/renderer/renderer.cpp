#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>

//
// constructor
//

core::code
load_shaders(VkDevice& dev, VkShaderModule& vs, VkShaderModule& fs)
{
  auto vs_result =
    mgmt::vulkan::pipeline::load_shader_module("./shaders/mesh.vert.spv", dev);
  if (!vs_result.has_value()) {
    ERR("error building the vertex shader");
    return core::code::ERROR;
  }
  vs = vs_result.value();
  auto fs_result =
    mgmt::vulkan::pipeline::load_shader_module("./shaders/mesh.frag.spv", dev);
  if (!fs_result.has_value()) {
    ERR("error building the fragment shader");
    return core::code::ERROR;
  }
  fs = fs_result.value();
  return core::code::SUCCESS;
}

core::code
render::AssetRenderer::init_pipelines()
{
  VkDevice dev = p_vkMgr_->get_dev();
  mgmt::vulkan::Swapchain& swapchain = *p_swapchain_;
  // load shaders
  VkShaderModule fs, vs;
  core::code status = load_shaders(dev, vs, fs);
  if (status != core::code::SUCCESS) {
    return status;
  }
  // scene layout
  mgmt::vulkan::descriptor::LayoutBuilder sceneLayoutBuilder;
  sceneLayoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  scene_layout_ =
    sceneLayoutBuilder
      .build(dev,
             VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
      .value(); // TODO: check ok and maybe abstract this
  // material layout
  mgmt::vulkan::descriptor::LayoutBuilder material_layout_builder;
  material_layout_builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  material_layout_builder.add_binding(
    1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  material_layout_builder.add_binding(
    2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  material_pipes_.layout =
    material_layout_builder
      .build(dev, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
      .value(); // TODO: check ok and maybe abstract this
  // descriptor set
  VkDescriptorSetLayout layouts[] = { scene_layout_, material_pipes_.layout };
  // range
  VkPushConstantRange range{};
  range.offset = 0;
  range.size = sizeof(asset::mesh::GPUMeshPushConstants);
  range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  // pipeline layout
  VkPipelineLayoutCreateInfo mesh_layout_info =
    mgmt::vulkan::info::pipeline_layout_create_info();
  mesh_layout_info.setLayoutCount = 2;
  mesh_layout_info.pSetLayouts = layouts;
  mesh_layout_info.pPushConstantRanges = &range;
  mesh_layout_info.pushConstantRangeCount = 1;
  // pipeline layout
  VkPipelineLayout pipeline_layout;
  status = mgmt::vulkan::check(
    vkCreatePipelineLayout(dev, &mesh_layout_info, nullptr, &pipeline_layout));
  if (status != core::code::SUCCESS) {
    ERR("error when building the pipeline layout");
    return core::code::ERROR;
  }
  material_pipes_.opaque_pipeline.layout = pipeline_layout;
  material_pipes_.transparent_pipeline.layout = pipeline_layout;
  // pipeline
  mgmt::vulkan::pipeline::Builder pipeline_builder;
  pipeline_builder.set_layout(pipeline_layout);
  pipeline_builder.set_shaders(vs, fs);
  pipeline_builder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
  pipeline_builder.set_polygon_mode(VK_POLYGON_MODE_FILL);
  pipeline_builder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
  pipeline_builder.set_multisampling_none();
  pipeline_builder.disable_blending();
  pipeline_builder.enable_depthtest(true, VK_COMPARE_OP_LESS_OR_EQUAL);
  pipeline_builder.set_color_attachment_format(swapchain.draw_img.format);
  pipeline_builder.set_depth_format(swapchain.depth_img.format);
  material_pipes_.opaque_pipeline.pipe =
    pipeline_builder.build_pipeline(dev).value(); // TODO: check errors
  pipeline_builder.enable_blending_additive();
  pipeline_builder.enable_depthtest(false, VK_COMPARE_OP_LESS_OR_EQUAL);
  material_pipes_.transparent_pipeline.pipe =
    pipeline_builder.build_pipeline(dev).value(); // TODO: check errors
  vkDestroyShaderModule(dev, fs, nullptr);
  vkDestroyShaderModule(dev, vs, nullptr);
  // del
  del_queue_.push([=]() {
    auto dev = p_vkMgr_->get_dev();
    vkDestroyPipeline(dev, material_pipes_.transparent_pipeline.pipe, nullptr);
    vkDestroyPipeline(dev, material_pipes_.opaque_pipeline.pipe, nullptr);
    vkDestroyPipelineLayout(dev, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(dev, material_pipes_.layout, nullptr);
    vkDestroyDescriptorSetLayout(dev, scene_layout_, nullptr);
  });
  return core::code::SUCCESS;
}

core::code
render::AssetRenderer::init_meshes()
{
  // // init mesh
  // auto meshes_result =
  //   asset::mesh::load_gltf_meshes(*vk_mgr_, "./res/meshes/basicmesh.glb");
  // if (!meshes_result.has_value()) {
  //   core::Logger::err("render::AssetRenderer::init_meshes",
  //                     "could not load meshes");
  //   return core::code::ERROR;
  // }
  // meshes_ = meshes_result.value();
  std::shared_ptr<asset::LoadedGLTF> scene_nodes =
    std::make_shared<asset::LoadedGLTF>();
  render::gltf::load_gltf("./res/meshes/structure.glb",
                          *p_vkMgr_,
                          default_res_,
                          writer_,
                          material_pipes_,
                          scene_nodes); // TODO errr handling
  loaded_scenes_["structure"] = scene_nodes;
  // del
  del_queue_.push([=]() { loaded_scenes_["structure"]->clearAll(); });
  return core::code::SUCCESS;
}

core::code
render::AssetRenderer::init_default_data()
{
  return default_res_.init(writer_, material_pipes_);
}

core::code
render::AssetRenderer::init_scene()
{
  // for (auto& m : meshes_) {
  //   std::shared_ptr<asset::MeshNode> new_node =
  //     std::make_shared<asset::MeshNode>();
  //   new_node->mesh = m;
  //   new_node->local_tf = m4f{ 1.f };
  //   new_node->world_tf = m4f{ 1.f };
  //   for (auto& s : new_node->mesh->surfaces) {
  //     s.material = std::make_shared<asset::material::Material>(
  //       default_res_.default_material_data);
  //   }
  //   loaded_nodes_[m->name] = std::move(new_node);
  // }
  // loaded_nodes_["Suzanne"]->Draw(m4f{ 1.f }, main_draw_ctx_);
  // m4f transform = m4f{ 1.0f };
  // for (int x = -3; x < 3; x++) {
  //   m4f scale = glm::scale(transform, v3f{ 0.2 });
  //   m4f translation = glm::translate(transform, v3f{ x, 1, 0 });
  //   loaded_nodes_["Cube"]->Draw(translation * scale, main_draw_ctx_);
  // }
  loaded_scenes_["structure"]->Draw(glm::mat4{ 1.f }, main_draw_ctx_);
  return core::code::SUCCESS;
}

core::code
render::AssetRenderer::init()
{
  if (initialized == core::status::INITIALIZED) {
    return core::code::SUCCESS;
  }
  if (initialized == core::status::ERROR) {
    return core::code::ERROR;
  }
  auto result = init_pipelines();
  if (result != core::code::SUCCESS) {
    return result;
  }
  result = init_default_data();
  if (result != core::code::SUCCESS) {
    return result;
  }
  result = init_meshes();
  if (result != core::code::SUCCESS) {
    return result;
  }
  result = init_scene();
  if (result != core::code::SUCCESS) {
    return result;
  }
  // success
  initialized = core::status::INITIALIZED;
  return core::code::SUCCESS;
}

render::AssetRenderer::AssetRenderer(mgmt::vulkan::Swapchain* mp_swapchain,
                                     mgmt::vulkan::Manager* mp_vkMgr)
  : render::Renderer{ mp_vkMgr, mp_swapchain }
  , default_res_{ mp_vkMgr } {};

//
// destructor
//

core::code
render::AssetRenderer::destroy()
{
  // todo@engine: handle pipes?
  p_vkMgr_->device_wait_idle();
  default_res_.destroy();
  del_queue_.flush();
  initialized = core::status::NOT_INITIALIZED;
  return core::code::SUCCESS;
}

render::AssetRenderer::~AssetRenderer()
{
  if (initialized == core::status::INITIALIZED) {
    destroy();
  }
}

//
// draw
//

void
render::AssetRenderer::draw(Camera& camera)
{
  mgmt::vulkan::Swapchain& swapchain = *p_swapchain_;
  auto cmd = swapchain.get_current_cmd_buffer();
  auto& current_frame = swapchain.get_current_frame();
  auto color_attachment = mgmt::vulkan::info::color_attachment_info(
    swapchain.draw_img.view, nullptr, VK_IMAGE_LAYOUT_GENERAL);
  auto depth_attachment = mgmt::vulkan::info::depth_attachment_info(
    swapchain.depth_img.view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
  auto render_info = mgmt::vulkan::info::rendering_info(
    swapchain.draw_extent, &color_attachment, &depth_attachment);
  vkCmdBeginRendering(cmd, &render_info);
  swapchain.set_viewport_and_sissor();
  // opaque
  auto scene_buffer = p_vkMgr_
                        ->create_buffer(sizeof(GPUSceneData),
                                        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                        VMA_MEMORY_USAGE_CPU_TO_GPU)
                        .value(); // TODO
  current_frame.del_queue.push(   // super inefficient
    [=, this]() { p_vkMgr_->destroy_buffer(scene_buffer); });
  void* data;
  vmaMapMemory(p_vkMgr_->get_allocator(), scene_buffer.allocation, &data);
  memcpy(data, &scene_, sizeof(scene_));
  vmaUnmapMemory(p_vkMgr_->get_allocator(), scene_buffer.allocation);
  VkDescriptorSet global_descriptor_set =
    current_frame.frame_descriptors.allocate(p_vkMgr_->get_dev(),
                                             scene_layout_);
  mgmt::vulkan::descriptor::Writer writer;
  writer.write_buffer(0,
                      scene_buffer.buffer,
                      sizeof(GPUSceneData),
                      0,
                      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  writer.update_set(p_vkMgr_->get_dev(), global_descriptor_set);
  asset::mesh::GPUMeshPushConstants push_constants;

  for (const render::asset::Object& draw : main_draw_ctx_.opaque_surfaces) {
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
    vkCmdBindIndexBuffer(cmd, draw.idx_buff, 0, VK_INDEX_TYPE_UINT32);
    push_constants.world_matrix = draw.transform;
    push_constants.vertex_buff_addr = draw.vertex_buff_addr;
    vkCmdPushConstants(cmd,
                       draw.material->pipe->layout,
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(asset::mesh::GPUMeshPushConstants),
                       &push_constants);
    vkCmdDrawIndexed(cmd, draw.idx_count, 1, draw.start_idx, 0, 0);
  }

  for (const render::asset::Object& draw :
       main_draw_ctx_.transparent_surfaces) {
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
    vkCmdBindIndexBuffer(cmd, draw.idx_buff, 0, VK_INDEX_TYPE_UINT32);
    push_constants.world_matrix = draw.transform;
    push_constants.vertex_buff_addr = draw.vertex_buff_addr;
    vkCmdPushConstants(cmd,
                       draw.material->pipe->layout,
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(asset::mesh::GPUMeshPushConstants),
                       &push_constants);
    vkCmdDrawIndexed(cmd, draw.idx_count, 1, draw.start_idx, 0, 0);
  }
  vkCmdEndRendering(cmd);
};

void
render::AssetRenderer::update_scene(Camera& camera)
{
  scene_.view = camera.view_matrix();
  // camera projection
  scene_.proj = camera.proj_matrix();
  scene_.proj[1][1] *= -1;
  scene_.viewproj = scene_.proj * scene_.view;
  // some default lighting parameters
  scene_.ambient = glm::vec4(.1f);
  scene_.sun_color = glm::vec4(1.f);
  scene_.sun_dir = glm::vec4(0, 1, 0.5, 1.f);
}
