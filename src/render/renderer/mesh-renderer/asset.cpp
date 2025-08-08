#include "types.h"

//
// Node
//

void
render::Node::refresh_transform(const m4f& parent)
{
  world = parent * local;
  for (auto c : children) {
    c->refresh_transform(world);
  }
}

void
render::Node::Draw(const m4f& top, DrawContext& ctx)
{
  // draw children
  for (auto& c : children) {
    c->Draw(top, ctx);
  }
}

//
// MeshNode
//

void
render::MeshNode::Draw(const m4f& top, render::DrawContext& ctx)
{
  m4f local = top * world;
  for (auto& s : p_mesh->surfaces) {
    Asset asset = {
      .idxCount = s.count,
      .startIdx = s.startIdx,
      .transform = local,
      .p_material = &s.p_material->data,
      .bounds = s.bounds,
      .p_idxBuff = p_mesh->buffers.idxBuff.buffer,
      .p_vba = p_mesh->buffers.vba,
    };
    if (s.p_material->data.type == MaterialPassType::opaque) {
      auto rawPtr = s.p_material.get();
      if (ctx.opaqueSurfaces.contains(s.p_material.get())) {
        ctx.opaqueSurfaces[rawPtr].push_back(asset);
      } else {
        ctx.opaqueSurfaces[rawPtr] = { asset };
      }
    } else {
      ctx.transparentSurfaces.push_back(asset);
    }
  }
  Node::Draw(top, ctx);
}
