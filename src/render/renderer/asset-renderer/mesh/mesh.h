#pragma once

#include "../material/material.h"
#include "../renderer.h"

namespace render {
namespace asset {

namespace mesh {

struct Vertex
{
  v3f position;
  f32 uv_x;
  v3f normal;
  f32 uv_y;
  v4f color;
};

struct Surface
{
  u32 start_idx;
  u32 count;
  std::shared_ptr<material::Material> material;
};

struct GPUMeshBuffers
{
  mgmt::vulkan::buffer::AllocatedBuffer index_buff;
  mgmt::vulkan::buffer::AllocatedBuffer vertex_buff;
  VkDeviceAddress vertex_buff_addr;
};

struct Mesh
{
  std::string name;
  std::vector<Surface> surfaces;
  GPUMeshBuffers mesh_buffers;
};

struct GPUMeshPushConstants
{
  glm::mat4 world_matrix;
  VkDeviceAddress vertex_buff_addr;
};

core::Result<GPUMeshBuffers, core::code>
upload_mesh(mgmt::vulkan::Manager& vk_mgr,
            std::span<u32> indices,
            std::span<Vertex> vertices);

core::Result<std::vector<std::shared_ptr<Mesh>>, core::code>
load_gltf_meshes(mgmt::vulkan::Manager& vk_mgr, char* path);

} // namespace mesh

} // namespace asset
} // namespace render
