#pragma once

#include "../material/material.h"
#include "../mesh/mesh.h"
#include "../renderer.h"

namespace render {
namespace asset {

class DefaultResources
{
private:
  mgmt::vulkan::Manager* vk_mgr_;
  DestructorQueue del_queue_;

public:
  bool initialized = false;

  mgmt::vulkan::image::AllocatedImage white_img;
  mgmt::vulkan::image::AllocatedImage black_img;
  mgmt::vulkan::image::AllocatedImage gray_img;
  mgmt::vulkan::image::AllocatedImage error_checker_img;
  VkSampler default_linear_sampler;
  VkSampler default_nearest_sampler;
  asset::material::Instance default_material_data;

  core::code init(mgmt::vulkan::descriptor::Writer& writer,
                  render::asset::material::MaterialPipelines& material_pipes);
  DefaultResources(mgmt::vulkan::Manager* vk_mgr);

  core::code destroy();
  ~DefaultResources();
};

struct Object
{
  u32 idx_count;
  u32 start_idx;
  material::Instance* material;
  m4f transform;
  VkBuffer idx_buff;
  VkDeviceAddress vertex_buff_addr;
};

struct DrawContext
{
  std::vector<Object> opaque_surfaces;
  std::vector<Object> transparent_surfaces;
};

class IRenderable
{
  virtual void Draw(const m4f& top, DrawContext& ctx) = 0;
};

struct Node : public IRenderable
{
  std::weak_ptr<Node> parent;
  std::vector<std::shared_ptr<Node>> children;
  m4f local_tf;
  m4f world_tf;

  void refresh_transform(const m4f& parent);
  virtual void Draw(const m4f& top, DrawContext& ctx);
};

struct MeshNode : public Node
{
  std::shared_ptr<mesh::Mesh> mesh;
  virtual void Draw(const m4f& top, DrawContext& ctx) override;
};

struct LoadedGLTF : public IRenderable
{
  bool initialized = false;
  std::unordered_map<std::string, std::shared_ptr<mesh::Mesh>> meshes;
  std::unordered_map<std::string, std::shared_ptr<Node>> nodes;
  std::unordered_map<std::string, mgmt::vulkan::image::AllocatedImage> images;
  std::unordered_map<std::string, std::shared_ptr<material::Material>>
    materials;
  std::vector<std::shared_ptr<Node>> top_nodes;
  std::vector<VkSampler> samplers;
  mgmt::vulkan::descriptor::DynamicAllocator descriptor_pool;
  mgmt::vulkan::buffer::AllocatedBuffer material_data_buff;
  mgmt::vulkan::Manager* vk_mgr;
  DestructorQueue del_queue_;

  ~LoadedGLTF()
  {
    if (initialized) {
      clearAll();
    }
  };
  virtual void Draw(const m4f& topMatrix, DrawContext& ctx) override;

  void clearAll();
};

} // namespace asset
} // namespace render
