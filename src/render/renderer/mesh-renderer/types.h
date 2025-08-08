#pragma once

#include "../renderer.h"
#include <unordered_map>

namespace render {

//
// material
//

enum MaterialPassType
{
  opaque,
  transparent,
};

struct MaterialPassPipelines
{
  mgmt::vulkan::pipeline::Pipeline opaque, transparent;
  VkDescriptorSetLayout layout;
};

struct Material
{
  struct Instance
  {
    MaterialPassType type;
    mgmt::vulkan::pipeline::Pipeline* pipeline;
    VkDescriptorSet p_materialSet;
  } data;
  u32 count;
  std::shared_ptr<Material> p_material;
};

struct MaterialResources
{
  mgmt::vulkan::image::AllocatedImage colorImg, metalImg;
  VkSampler p_colorSampler, p_metalSampler;
  VkBuffer p_dataBuff;
  u32 dataBuffOffset;
};

struct GPUMaterialConstants
{
  v4f colorFactors, metalRoughFactors;
  v4f padding[14];
};

Material::Instance
write_material(MaterialPassType pass,
               MaterialResources const& ir_resources,
               mgmt::vulkan::Manager& ip_vkMgr,
               mgmt::vulkan::descriptor::DynamicAllocator& ir_allocator,
               mgmt::vulkan::descriptor::Writer& ir_writer,
               MaterialPassPipelines& ir_pipes);

//
// mesh
//

struct MeshVertex
{
  v3f position;
  f32 uvX;
  v3f normal;
  f32 uvY;
  v4f color;
};

struct MeshBounds
{
  v3f origin;
  f32 sphereRadius;
  v3f extents;
};

struct MeshSurface
{
  u32 startIdx, count;
  MeshBounds bounds;
  std::shared_ptr<Material> p_material;
};

struct GPUMeshBuffers
{
  mgmt::vulkan::buffer::AllocatedBuffer idxBuff, vertexBuff;
  VkDeviceAddress vba;
};

struct Mesh
{
  std::string name;
  std::vector<MeshSurface> surfaces;
  GPUMeshBuffers buffers;
};

struct GPUMeshPushConstants
{
  m4f world;
  VkDeviceAddress vba;
};

//
// asset
//

struct Asset
{
  u32 idxCount, startIdx;
  m4f transform;
  Material::Instance* p_material;
  MeshBounds bounds;
  VkBuffer p_idxBuff;
  VkDeviceAddress p_vba;
};

struct DrawContext
{
  std::unordered_map<Material*, std::vector<Asset>> opaqueSurfaces;
  std::vector<Asset> transparentSurfaces;
};

class IRenderable
{
  virtual void Draw(const m4f& top, DrawContext& ctx) = 0;
};

struct Node : public IRenderable
{
  std::weak_ptr<Node> p_parent;
  std::vector<std::shared_ptr<Node>> children;
  m4f local, world;

  void refresh_transform(const m4f& ir_parent);
  virtual void Draw(const m4f& ir_top, DrawContext& mr_ctx);
};

struct MeshNode : public Node
{
  std::shared_ptr<Mesh> p_mesh;
  virtual void Draw(const m4f& ir_top, DrawContext& mr_ctx) override;
};

struct DefaultResources
{
  mgmt::vulkan::image::AllocatedImage whiteImg, blackImg, grayImg, errorImg;
  VkSampler p_linearSampler, p_nearestSampler;
  Material::Instance defaultMaterialData;
};

} // namespace render
