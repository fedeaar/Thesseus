#include "renderer.h"
#include "gltf/gltf.h"
#include "types.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan_core.h>

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
  sceneLayout_ =
    sceneLayoutBuilder
      .build(dev,
             VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
      .value(); // TODO: check ok and maybe abstract this
  // material layout
  mgmt::vulkan::descriptor::LayoutBuilder materialLayoutBuilder;
  materialLayoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  materialLayoutBuilder.add_binding(1,
                                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  materialLayoutBuilder.add_binding(2,
                                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
  materialPipes_.layout =
    materialLayoutBuilder
      .build(dev, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
      .value(); // TODO: check ok and maybe abstract this
  // descriptor set
  VkDescriptorSetLayout layouts[] = { sceneLayout_, materialPipes_.layout };
  // range
  VkPushConstantRange range{
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .offset = 0,
    .size = sizeof(GPUMeshPushConstants),
  };
  // pipeline layout
  VkPipelineLayoutCreateInfo meshLayoutInfo =
    mgmt::vulkan::info::pipeline_layout_create_info();
  meshLayoutInfo.setLayoutCount = 2;
  meshLayoutInfo.pSetLayouts = layouts;
  meshLayoutInfo.pPushConstantRanges = &range;
  meshLayoutInfo.pushConstantRangeCount = 1;
  // pipeline layout
  VkPipelineLayout pipelineLayout;
  status = mgmt::vulkan::check( // TODO: this should not be here
    vkCreatePipelineLayout(dev, &meshLayoutInfo, nullptr, &pipelineLayout));
  if (status != core::code::SUCCESS) {
    ERR("error when building the pipeline layout");
    return core::code::ERROR;
  }
  materialPipes_.opaque.layout = pipelineLayout;
  materialPipes_.transparent.layout = pipelineLayout;
  // pipeline
  mgmt::vulkan::pipeline::Builder pipelineBuilder;
  pipelineBuilder.set_layout(pipelineLayout);
  pipelineBuilder.set_shaders(vs, fs);
  pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
  pipelineBuilder.set_polygon_mode(VK_POLYGON_MODE_FILL);
  pipelineBuilder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
  pipelineBuilder.set_multisampling_none();
  pipelineBuilder.disable_blending();
  pipelineBuilder.enable_depthtest(true, VK_COMPARE_OP_LESS_OR_EQUAL);
  pipelineBuilder.set_color_attachment_format(swapchain.draw_img.format);
  pipelineBuilder.set_depth_format(swapchain.depth_img.format);
  materialPipes_.opaque.pipeline =
    pipelineBuilder.build_pipeline(dev).value(); // TODO: check errors
  pipelineBuilder.enable_blending_additive();
  pipelineBuilder.enable_depthtest(false, VK_COMPARE_OP_LESS_OR_EQUAL);
  materialPipes_.transparent.pipeline =
    pipelineBuilder.build_pipeline(dev).value(); // TODO: check errors
  vkDestroyShaderModule(dev, fs, nullptr);
  vkDestroyShaderModule(dev, vs, nullptr);
  // del
  delQueue_.push([=]() {
    auto dev = p_vkMgr_->get_dev();
    vkDestroyPipeline(dev, materialPipes_.transparent.pipeline, nullptr);
    vkDestroyPipeline(dev, materialPipes_.opaque.pipeline, nullptr);
    vkDestroyPipelineLayout(dev, pipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(dev, materialPipes_.layout, nullptr);
    vkDestroyDescriptorSetLayout(dev, sceneLayout_, nullptr);
  });
  return core::code::SUCCESS;
}

core::code
render::AssetRenderer::init_default_data()
{
  auto& vkMgr = *p_vkMgr_;
  // default imgs
  u32 white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
  if (vkMgr.create_image((void*)&white,
                         VkExtent3D{ 1, 1, 1 },
                         VK_FORMAT_R8G8B8A8_UNORM,
                         VK_IMAGE_USAGE_SAMPLED_BIT,
                         false,
                         defaultRes_.whiteImg) != core::code::SUCCESS) {
    ERR("could not load white img");
    return core::code::ERROR;
  }
  u32 gray = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
  if (vkMgr.create_image((void*)&gray,
                         VkExtent3D{ 1, 1, 1 },
                         VK_FORMAT_R8G8B8A8_UNORM,
                         VK_IMAGE_USAGE_SAMPLED_BIT,
                         false,
                         defaultRes_.grayImg) != core::code::SUCCESS) {
    ERR("could not load gray img");
    return core::code::ERROR;
  }
  u32 black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 0));
  if (vkMgr.create_image((void*)&black,
                         VkExtent3D{ 1, 1, 1 },
                         VK_FORMAT_R8G8B8A8_UNORM,
                         VK_IMAGE_USAGE_SAMPLED_BIT,
                         false,
                         defaultRes_.blackImg) != core::code::SUCCESS) {
    ERR("could not load black img");
    return core::code::ERROR;
  }
  u32 magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
  std::array<u32, 16 * 16> pixels;
  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
    }
  }
  if (vkMgr.create_image(pixels.data(),
                         VkExtent3D{ 16, 16, 1 },
                         VK_FORMAT_R8G8B8A8_UNORM,
                         VK_IMAGE_USAGE_SAMPLED_BIT,
                         false,
                         defaultRes_.errorImg) != core::code::SUCCESS) {
    ERR("could not load checker img");
    return core::code::ERROR;
  }
  // default samplers
  VkSamplerCreateInfo samplerInfo = { .sType =
                                        VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
  samplerInfo.magFilter = VK_FILTER_NEAREST;
  samplerInfo.minFilter = VK_FILTER_NEAREST;
  vkCreateSampler(
    vkMgr.get_dev(), &samplerInfo, nullptr, &defaultRes_.p_nearestSampler);
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  vkCreateSampler(
    vkMgr.get_dev(), &samplerInfo, nullptr, &defaultRes_.p_linearSampler);
  // default material textures
  MaterialResources resources = { .colorImg = defaultRes_.errorImg,
                                  .metalImg = defaultRes_.errorImg,
                                  .p_colorSampler =
                                    defaultRes_.p_nearestSampler,
                                  .p_metalSampler =
                                    defaultRes_.p_nearestSampler,
                                  .dataBuffOffset = 0 };
  mgmt::vulkan::buffer::AllocatedBuffer constants =
    vkMgr
      .create_buffer(sizeof(GPUMaterialConstants),
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VMA_MEMORY_USAGE_CPU_TO_GPU)
      .value(); // TODO: error handling
  GPUMaterialConstants sceneConstants;
  sceneConstants.colorFactors = v4f{ 1, 1, 1, 1 };
  sceneConstants.metalRoughFactors = v4f{ 1, 0.5, 0, 0 };
  void* p_sceneConstants;
  vmaMapMemory(vkMgr.get_allocator(), constants.allocation, &p_sceneConstants);
  memcpy(p_sceneConstants, &sceneConstants, sizeof(GPUMaterialConstants));
  vmaUnmapMemory(vkMgr.get_allocator(), constants.allocation);
  resources.p_dataBuff = constants.buffer;
  resources.dataBuffOffset = 0;
  defaultRes_.defaultMaterialData =
    render::write_material(MaterialPassType::opaque,
                           resources,
                           vkMgr,
                           vkMgr.get_global_descriptor_allocator(),
                           writer_,
                           materialPipes_);
  // del
  delQueue_.push([=, this]() {
    auto& vkMgr = *p_vkMgr_;
    auto dev = vkMgr.get_dev();
    vkMgr.destroy_buffer(constants);
    vkDestroySampler(dev, defaultRes_.p_linearSampler, nullptr);
    vkDestroySampler(dev, defaultRes_.p_nearestSampler, nullptr);
  });
  return core::code::SUCCESS;
}

core::code
render::AssetRenderer::init_scenes()
{
  std::shared_ptr<gltf::GLTFScene> p_scene = std::make_shared<gltf::GLTFScene>(
    gltf::GLTFScene{ "./res/meshes/structure.glb", p_vkMgr_ });
  if (p_scene->init(defaultRes_, writer_, materialPipes_) !=
      core::code::SUCCESS) {
    ERR("failed to load scene");
    return core::code::ERROR;
  }
  loadedScenes_["structure"] = p_scene;
  p_scene->Draw(m4f{ 1.f }, mainDrawCtx_);
  delQueue_.push([=]() { loadedScenes_["structure"]->destroy(); });
  return core::code::SUCCESS;
}

core::code
render::AssetRenderer::init()
{
  if (initialized == core::status::INITIALIZED) {
    return core::code::SUCCESS;
  }
  if (initialized == core::status::ERROR) {
    return core::code::IN_ERROR_STATE;
  }
  auto result = init_pipelines();
  if (result != core::code::SUCCESS) {
    return result;
  }
  result = init_default_data();
  if (result != core::code::SUCCESS) {
    return result;
  }
  result = init_scenes();
  if (result != core::code::SUCCESS) {
    return result;
  }
  // success
  initialized = core::status::INITIALIZED;
  return core::code::SUCCESS;
}

render::AssetRenderer::AssetRenderer(mgmt::vulkan::Swapchain* p_swapchain,
                                     mgmt::vulkan::Manager* p_vkMgr,
                                     debug::GlobalStats* p_stats)
  : render::Renderer{ p_vkMgr, p_swapchain, p_stats } {};

//
// destructor
//

core::code
render::AssetRenderer::destroy()
{
  if (initialized == core::status::NOT_INITIALIZED) {
    return core::code::SUCCESS;
  }
  if (initialized == core::status::ERROR) {
    return core::code::IN_ERROR_STATE;
  }
  // todo@engine: handle pipes?
  p_vkMgr_->device_wait_idle();
  delQueue_.flush();
  initialized = core::status::NOT_INITIALIZED;
  return core::code::SUCCESS;
}

render::AssetRenderer::~AssetRenderer()
{
  destroy();
}

//
// draw
//

std::array<v3f, 8> frustrumCorners{
  v3f{ 1, 1, 1 },  v3f{ 1, 1, -1 },  v3f{ 1, -1, 1 },  v3f{ 1, -1, -1 },
  v3f{ -1, 1, 1 }, v3f{ -1, 1, -1 }, v3f{ -1, -1, 1 }, v3f{ -1, -1, -1 },
};

inline bool
visible(render::Asset const& ir_asset, m4f const& viewproj)
{
  m4f matrix = viewproj * ir_asset.transform;
  v3f min = { 1.5, 1.5, 1.5 };
  v3f max = { -1.5, -1.5, -1.5 };
  auto& origin = ir_asset.bounds.origin;
  auto& extents = ir_asset.bounds.extents;
  for (u32 c = 0; c < 8; ++c) {
    glm::vec4 v =
      matrix * glm::vec4(origin + (frustrumCorners[c] * extents), 1.f);
    // perspective correction
    v.x = v.x / v.w;
    v.y = v.y / v.w;
    v.z = v.z / v.w;
    min = glm::min(v3f{ v.x, v.y, v.z }, min);
    max = glm::max(v3f{ v.x, v.y, v.z }, max);
  }
  return !(min.z > 1.f || max.z < 0.f || min.x > 1.f || max.x < -1.f ||
           min.y > 1.f || max.y < -1.f);
}

void
render::AssetRenderer::update_scene(Camera& camera)
{
  sceneData_.view = camera.view_matrix();
  // camera projection
  sceneData_.proj = camera.proj_matrix();
  sceneData_.proj[1][1] *= -1;
  sceneData_.viewproj = sceneData_.proj * sceneData_.view;
  // some default lighting parameters
  sceneData_.ambient = glm::vec4(.1f);
  sceneData_.sunColor = glm::vec4(1.f);
  sceneData_.sunDir = glm::vec4(0, 1, 0.5, 1.f);
}

inline void
cmd_draw(render::Asset const& ir_asset,
         VkDescriptorSet& ir_descriptorSet,
         VkCommandBuffer& mr_cmd,
         render::GPUMeshPushConstants& mr_pushConstants,
         mgmt::vulkan::pipeline::Pipeline* mr_lastPipeline,
         render::Material::Instance* mr_lastMaterial,
         VkBuffer mr_lastIndexBuffer)
{
  auto& pipeline = ir_asset.p_material->pipeline;
  if (ir_asset.p_material != mr_lastMaterial) {
    mr_lastMaterial = ir_asset.p_material;
    if (pipeline != mr_lastPipeline) {
      mr_lastPipeline = pipeline;
      vkCmdBindPipeline(
        mr_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
      vkCmdBindDescriptorSets(mr_cmd,
                              VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pipeline->layout,
                              0,
                              1,
                              &ir_descriptorSet,
                              0,
                              nullptr);
    }
    vkCmdBindDescriptorSets(mr_cmd,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline->layout,
                            1,
                            1,
                            &ir_asset.p_material->p_materialSet,
                            0,
                            nullptr);
  }
  if (ir_asset.p_idxBuff != mr_lastIndexBuffer) {
    mr_lastIndexBuffer = ir_asset.p_idxBuff;
    vkCmdBindIndexBuffer(mr_cmd, ir_asset.p_idxBuff, 0, VK_INDEX_TYPE_UINT32);
  }
  mr_pushConstants.world = ir_asset.transform;
  mr_pushConstants.vba = ir_asset.p_vba;
  vkCmdPushConstants(mr_cmd,
                     ir_asset.p_material->pipeline->layout,
                     VK_SHADER_STAGE_VERTEX_BIT,
                     0,
                     sizeof(render::GPUMeshPushConstants),
                     &mr_pushConstants);
  vkCmdDrawIndexed(mr_cmd, ir_asset.idxCount, 1, ir_asset.startIdx, 0, 0);
}

void
render::AssetRenderer::draw(Camera& camera)
{
  auto start = std::chrono::system_clock::now();
  // update
  update_scene(camera);
  auto& swapchain = *p_swapchain_;
  auto& cmd = swapchain.get_current_cmd_buffer();
  auto& frame = swapchain.get_current_frame();
  auto colorAttachment = mgmt::vulkan::info::color_attachment_info(
    swapchain.draw_img.view, nullptr, VK_IMAGE_LAYOUT_GENERAL);
  auto depthAttachment = mgmt::vulkan::info::depth_attachment_info(
    swapchain.depth_img.view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
  auto renderInfo = mgmt::vulkan::info::rendering_info(
    swapchain.draw_extent, &colorAttachment, &depthAttachment);
  // render
  vkCmdBeginRendering(cmd, &renderInfo);
  swapchain.set_viewport_and_sissor();
  // opaque
  auto sceneBuffer = p_vkMgr_
                       ->create_buffer(sizeof(GPUSceneData),
                                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                       VMA_MEMORY_USAGE_CPU_TO_GPU)
                       .value(); // TODO
  frame.del_queue.push(          // FIXME: super inefficient
    [=, this]() { p_vkMgr_->destroy_buffer(sceneBuffer); });
  void* data;
  vmaMapMemory(p_vkMgr_->get_allocator(), sceneBuffer.allocation, &data);
  memcpy(data, &sceneData_, sizeof(sceneData_));
  vmaUnmapMemory(p_vkMgr_->get_allocator(), sceneBuffer.allocation);
  VkDescriptorSet globalDescriptorSet =
    frame.frame_descriptors.allocate(p_vkMgr_->get_dev(), sceneLayout_);
  mgmt::vulkan::descriptor::Writer writer;
  writer.write_buffer(0,
                      sceneBuffer.buffer,
                      sizeof(GPUSceneData),
                      0,
                      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  writer.update_set(p_vkMgr_->get_dev(), globalDescriptorSet);
  GPUMeshPushConstants pushConstants;
  u32 drawCount = 0;
  u32 triangleCount = 0;
  mgmt::vulkan::pipeline::Pipeline* lastPipeline = nullptr;
  render::Material::Instance* lastMaterial = nullptr;
  VkBuffer lastIndexBuffer = VK_NULL_HANDLE;
  for (auto& drawMaterial : mainDrawCtx_.opaqueSurfaces) {
    for (auto& draw : drawMaterial.second) {
      if (!visible(draw, sceneData_.viewproj)) {
        continue;
      }
      cmd_draw(draw,
               globalDescriptorSet,
               cmd,
               pushConstants,
               lastPipeline,
               lastMaterial,
               lastIndexBuffer);
      drawCount++;
      triangleCount += draw.idxCount / 3;
    }
  }
  for (const Asset& draw : mainDrawCtx_.transparentSurfaces) {
    if (!visible(draw, sceneData_.viewproj)) {
      continue;
    }
    cmd_draw(draw,
             globalDescriptorSet,
             cmd,
             pushConstants,
             lastPipeline,
             lastMaterial,
             lastIndexBuffer);
    drawCount++;
    triangleCount += draw.idxCount / 3;
  }
  p_stats_->drawcallCount = drawCount;
  p_stats_->triangleCount = triangleCount;
  vkCmdEndRendering(cmd);
  auto end = std::chrono::system_clock::now();
  auto elapsed =
    std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  p_stats_->meshDrawTime = elapsed.count() / 1000.f;
};
