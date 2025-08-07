#pragma once

#include "../types.h"

namespace render {
namespace gltf {

class GLTFScene : public IRenderable
{
private:
  char* p_path_;
  mgmt::vulkan::Manager* p_vkMgr_;
  DestructorQueue delQueue_;

public:
  // status
  core::status initialized = core::status::NOT_INITIALIZED;
  // data
  std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
  std::unordered_map<std::string, std::shared_ptr<Node>> nodes;
  std::unordered_map<std::string, mgmt::vulkan::image::AllocatedImage> images;
  std::unordered_map<std::string, std::shared_ptr<Material>> materials;
  std::vector<std::shared_ptr<Node>> root;
  std::vector<VkSampler> samplers;
  mgmt::vulkan::descriptor::DynamicAllocator descriptorPool;
  mgmt::vulkan::buffer::AllocatedBuffer materialDataBuff;

  core::code init(render::DefaultResources& ir_defaultRes,
                  mgmt::vulkan::descriptor::Writer& ir_writer,
                  render::MaterialPassPipelines& ir_materialPipes);
  GLTFScene(char* p_path, mgmt::vulkan::Manager* p_vkMgr);

  core::code destroy();
  ~GLTFScene();

  virtual void Draw(const m4f& top, DrawContext& ctx) override;

private:
  core::code init_images(
    fastgltf::Asset& ir_asset,
    mgmt::vulkan::image::AllocatedImage& ir_default_image,
    std::vector<mgmt::vulkan::image::AllocatedImage>& mr_images);
  core::code init_samplers(fastgltf::Asset& ir_asset);
  core::code init_pools(fastgltf::Asset& ir_asset);
  core::code init_materials(
    mgmt::vulkan::descriptor::Writer& ir_writer,
    render::MaterialPassPipelines& ir_materialPipes,
    render::DefaultResources& ir_defaultRes,
    fastgltf::Asset& ir_asset,
    std::vector<mgmt::vulkan::image::AllocatedImage>& ir_images,
    std::vector<std::shared_ptr<render::Material>>& mr_materials);
  core::code init_meshes(
    fastgltf::Asset& ir_asset,
    std::vector<std::shared_ptr<render::Material>>& ir_materials,
    std::vector<std::shared_ptr<render::Mesh>>& mr_meshes);
  core::code init_nodes(fastgltf::Asset& ir_asset,
                        std::vector<std::shared_ptr<render::Mesh>>& ir_meshes,
                        std::vector<std::shared_ptr<render::Node>>& mr_nodes);
};

} // gltf
} // render
