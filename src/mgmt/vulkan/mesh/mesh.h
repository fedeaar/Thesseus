#pragma once

#include "../buffer/buffer.h"
#include "../info/info.h"
#include "../manager.h"
#include "../material/material.h"

#include <memory>

namespace mgmt {
namespace vulkan {

namespace mesh {

struct Vertex
{
  glm::vec3 position;
  float uv_x;
  glm::vec3 normal;
  float uv_y;
  glm::vec4 color;
};

struct GeoSurface
{
  u32 start_idx;
  u32 count;
  std::shared_ptr<material::Material> material;
};

struct GPUMeshBuffers
{
  buffer::AllocatedBuffer index_buff;
  buffer::AllocatedBuffer vertex_buff;
  VkDeviceAddress vertex_buff_addr;
};

struct MeshAsset
{
  std::string name;
  std::vector<GeoSurface> surfaces;
  GPUMeshBuffers mesh_buffers;
};

struct GPUDrawPushConstants
{
  glm::mat4 world_matrix;
  VkDeviceAddress vertex_buff_addr;
};

} // namespace mesh

} // namespace vulkan
} // namespace mgmt
