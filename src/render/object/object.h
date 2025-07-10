#pragma once

#include "../render.h"

namespace render {

struct Object
{
  u32 idx_count;
  u32 start_idx;
  VkBuffer idx_buff;
  mgmt::vulkan::material::Instance* material;
  m4f transform;
  VkDeviceAddress vertex_buff_addr;
};

struct DrawContext
{
  std::vector<Object> opaque_surfaces;
};

class IRenderable
{
  virtual void Draw(const m4f& topMatrix, DrawContext& ctx) = 0;
};

struct Node : public IRenderable
{
  std::weak_ptr<Node> parent;
  std::vector<std::shared_ptr<Node>> children;
  m4f local_tf;
  m4f world_tf;

  void refresh_transform(const m4f& parentMatrix)
  {
    world_tf = parentMatrix * local_tf;
    for (auto c : children) {
      c->refresh_transform(world_tf);
    }
  }

  virtual void Draw(const m4f& topMatrix, DrawContext& ctx)
  {
    // draw children
    for (auto& c : children) {
      c->Draw(topMatrix, ctx);
    }
  }
};

struct MeshNode : public Node
{
  std::shared_ptr<mgmt::vulkan::mesh::MeshAsset> mesh;
  virtual void Draw(const m4f& top, DrawContext& ctx) override
  {
    m4f nodeMatrix = top * world_tf;
    for (auto& s : mesh->surfaces) {
      Object def;
      def.idx_count = s.count;
      def.start_idx = s.start_idx;
      def.idx_buff = mesh->mesh_buffers.index_buff.buffer;
      def.material = &s.material->data;
      def.transform = nodeMatrix;
      def.vertex_buff_addr = mesh->mesh_buffers.vertex_buff_addr;
      ctx.opaque_surfaces.push_back(def);
    }
    Node::Draw(top, ctx);
  }
};

} // namespace render
