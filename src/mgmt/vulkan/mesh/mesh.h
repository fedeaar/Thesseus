#pragma once

#include "../buffer/buffer.h"
#include "../info/info.h"
#include "../manager.h"

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

struct GPUMeshBuffers
{
  buffer::AllocatedBuffer index_buff;
  buffer::AllocatedBuffer vertex_buff;
  VkDeviceAddress vertex_buff_addr;
};

struct GPUDrawPushConstants
{
  glm::mat4 world_matrix;
  VkDeviceAddress vertex_buff_addr;
};

} // namespace buffer

} // namespace vulkan
} // namespace mgmt
