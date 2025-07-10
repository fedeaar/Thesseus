#pragma once

#include "../asset-renderer/renderer.h"
#include "../renderer.h"

namespace render {

class GLTF_MetallicRoughnessRenderer : Renderer
{
public:
  bool initialized = false;

  mgmt::vulkan::material::Pipeline opaque_pipe_;
  mgmt::vulkan::material::Pipeline transparent_pipe_;
  VkDescriptorSetLayout material_layout_;

  struct MaterialConstants
  {
    v4f color_factors;
    v4f metal_rough_factors;
    // padding, we need it anyway for uniform buffers
    v4f extra[14];
  };
  DestructorQueue del_queue_;
  struct MaterialResources
  {
    mgmt::vulkan::image::AllocatedImage colorImage;
    VkSampler colorSampler;
    mgmt::vulkan::image::AllocatedImage metalRoughImage;
    VkSampler metalRoughSampler;
    VkBuffer dataBuffer;
    uint32_t dataBufferOffset;
  };
  struct GPUSceneData
  {
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewproj;
    glm::vec4 ambient;
    glm::vec4 sun_dir;
    glm::vec4 sun_color;
  } scene_;
  VkDescriptorSetLayout scene_layout_;
  mgmt::vulkan::descriptor::Writer writer_;
  mgmt::vulkan::material::Instance default_data_;

  render::DrawContext main_draw_ctx_;
  std::unordered_map<std::string, std::shared_ptr<render::Node>> loaded_nodes_;

  core::code init(mgmt::vulkan::Swapchain& swapchain,
                  render::AssetRenderer& asset_renderer);
  GLTF_MetallicRoughnessRenderer(mgmt::vulkan::Manager* vk_mgr);

  core::code destroy();
  ~GLTF_MetallicRoughnessRenderer();

  mgmt::vulkan::material::Instance write_material(
    mgmt::vulkan::material::Type pass,
    const MaterialResources& resources,
    mgmt::vulkan::descriptor::DynamicAllocator& descriptor_allocator);

  void draw(mgmt::vulkan::Swapchain& swapchain, Camera& camera);
  void update_scene();
};

} // namespace render
