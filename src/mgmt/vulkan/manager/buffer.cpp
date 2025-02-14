#include "manager.h"

//
// create
//

core::Result<mgmt::vulkan::buffer::AllocatedBuffer, core::code>
mgmt::vulkan::Manager::create_buffer(size_t size,
                                     VkBufferUsageFlags flags,
                                     VmaMemoryUsage usage)
{
  VkBufferCreateInfo buffer_info = { .sType =
                                       VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
  buffer_info.pNext = nullptr;
  buffer_info.size = size;
  buffer_info.usage = flags;
  VmaAllocationCreateInfo alloc_info = {};
  alloc_info.usage = usage;
  alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
  buffer::AllocatedBuffer buffer;
  auto status = check(vmaCreateBuffer(allocator_,
                                      &buffer_info,
                                      &alloc_info,
                                      &buffer.buffer,
                                      &buffer.allocation,
                                      &buffer.info));
  if (status != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::create_buffer",
                      "vmaCreateBuffer error");
    return core::code::ERROR;
  }
  return buffer;
}

//
// destroy
//

core::code
mgmt::vulkan::Manager::destroy_buffer(
  mgmt::vulkan::buffer::AllocatedBuffer const& buffer)
{
  vmaDestroyBuffer(allocator_, buffer.buffer, buffer.allocation);
  return core::code::SUCCESS;
}

//
// mesh
//

core::Result<mgmt::vulkan::mesh::GPUMeshBuffers, core::code>
mgmt::vulkan::Manager::upload_mesh(std::span<u32> indices,
                                   std::span<mesh::Vertex> vertices)
{
  const size_t vb_s = vertices.size() * sizeof(mesh::Vertex);
  const size_t ib_s = indices.size() * sizeof(u32);
  mesh::GPUMeshBuffers mesh;
  auto vertex_buff_result = create_buffer(
    vb_s,
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY);
  if (!vertex_buff_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::upload_mesh",
                      "could not create vertex buffer");
    return core::code::ERROR;
  }
  mesh.vertex_buff = vertex_buff_result.value();
  VkBufferDeviceAddressInfo addr_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
    .buffer = mesh.vertex_buff.buffer
  };
  mesh.vertex_buff_addr = vkGetBufferDeviceAddress(device_, &addr_info);
  auto index_buff_result = create_buffer(ib_s,
                                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                           VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                         VMA_MEMORY_USAGE_GPU_ONLY);
  if (!index_buff_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::upload_mesh",
                      "could not create index buffer");
    return core::code::ERROR;
  }
  mesh.index_buff = index_buff_result.value();
  auto staging_result = create_buffer(
    vb_s + ib_s, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
  if (!index_buff_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::upload_mesh",
                      "could not create staging buffer");
    return core::code::ERROR;
  }
  auto staging = staging_result.value();
  void* data;
  vmaMapMemory(allocator_, staging.allocation, &data);
  memcpy(data, vertices.data(), vb_s);
  memcpy((char*)data + vb_s, indices.data(), ib_s);
  vmaUnmapMemory(allocator_, staging.allocation);
  imm_submit([&](VkCommandBuffer cmd) {
    VkBufferCopy vertex_copy{ 0 };
    vertex_copy.dstOffset = 0;
    vertex_copy.srcOffset = 0;
    vertex_copy.size = vb_s;
    vkCmdCopyBuffer(
      cmd, staging.buffer, mesh.vertex_buff.buffer, 1, &vertex_copy);
    VkBufferCopy index_copy{ 0 };
    index_copy.dstOffset = 0;
    index_copy.srcOffset = vb_s;
    index_copy.size = ib_s;
    vkCmdCopyBuffer(
      cmd, staging.buffer, mesh.index_buff.buffer, 1, &index_copy);
  });
  destroy_buffer(staging);
  return mesh;
};

core::Result<std::vector<std::shared_ptr<mgmt::vulkan::mesh::MeshAsset>>,
             core::code>
mgmt::vulkan::Manager::load_gltf_meshes(char* path)
{
  fastgltf::Asset asset;
  if (core::io::gltf::load(path, &asset) != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::load_gltf_meshes",
                      "could not load gltf asset");
    return core::code::ERROR;
  }
  std::vector<std::shared_ptr<mesh::MeshAsset>> meshes;
  std::vector<u32> indices;
  std::vector<mesh::Vertex> vertices;
  for (auto& gltf_mesh : asset.meshes) {
    mesh::MeshAsset new_mesh;
    new_mesh.name = gltf_mesh.name;
    for (auto&& it : gltf_mesh.primitives) {
      mesh::GeoSurface new_surface;
      new_surface.start_idx = (u32)indices.size();
      size_t initial_vtx = vertices.size();

      // load indices
      if (!it.indicesAccessor.has_value()) {
        // todo: better handling
        core::Logger::err("mgmt::vulkan::Manager::load_gltf_meshes",
                          "invalid gltf file");
        return core::code::ERROR;
      }
      auto& idx_acc = asset.accessors[it.indicesAccessor.value()];
      new_surface.count = (u32)idx_acc.count;
      indices.reserve(indices.size() + idx_acc.count);
      fastgltf::iterateAccessor<u32>(
        asset, idx_acc, [&](u32 idx) { indices.push_back(idx + initial_vtx); });
      // load vertex positions
      auto pos = it.findAttribute("POSITION");
      if (pos == it.attributes.end()) {
        // todo: better handling
        core::Logger::err("mgmt::vulkan::Manager::load_gltf_meshes",
                          "invalid gltf file");
        return core::code::ERROR;
      }
      auto& pos_acc = asset.accessors[pos->accessorIndex];
      if (!pos_acc.bufferViewIndex.has_value()) {
        continue;
      }
      vertices.resize(vertices.size() + pos_acc.count);
      fastgltf::iterateAccessorWithIndex<v3f>(
        asset, pos_acc, [&](v3f v, size_t index) {
          mesh::Vertex vtx;
          vtx.position = v;
          vtx.normal = { 1, 0, 0 };
          vtx.color = v4f{ 1.f };
          vtx.uv_x = 0;
          vtx.uv_y = 0;
          vertices[initial_vtx + index] = vtx;
        });
      // load vertex normals
      auto normals = it.findAttribute("NORMAL");
      if (normals != it.attributes.end()) {
        auto& normals_acc = asset.accessors[normals->accessorIndex];
        fastgltf::iterateAccessorWithIndex<v3f>(
          asset, normals_acc, [&](v3f v, size_t index) {
            vertices[initial_vtx + index].normal = v;
          });
      }
      // load UVs
      auto uv = it.findAttribute("TEXCOORD_0");
      if (uv != it.attributes.end()) {
        auto& uv_acc = asset.accessors[uv->accessorIndex];
        fastgltf::iterateAccessorWithIndex<v2f>(
          asset, uv_acc, [&](v2f v, size_t index) {
            vertices[initial_vtx + index].uv_x = v.x;
            vertices[initial_vtx + index].uv_y = v.y;
          });
      }
      // load vertex colors
      auto colors = it.findAttribute("COLOR_0");
      if (colors != it.attributes.end()) {
        auto& colors_acc = asset.accessors[colors->accessorIndex];
        fastgltf::iterateAccessorWithIndex<v4f>(
          asset, colors_acc, [&](v4f v, size_t index) {
            vertices[initial_vtx + index].color = v;
          });
      }
      new_mesh.surfaces.push_back(new_surface);
    }
    // display the vertex normals
    constexpr bool override_colors = true;
    if (override_colors) {
      for (mesh::Vertex& vtx : vertices) {
        vtx.color = glm::vec4(vtx.normal, 1.f);
      }
    }
    auto upload_result = upload_mesh(indices, vertices);
    if (!upload_result.has_value()) {
      core::Logger::err("mgmt::vulkan::Manager::load_gltf_meshes",
                        "could not load gltf asset to gpu");
      return core::code::ERROR;
    }
    new_mesh.mesh_buffers = upload_result.value();
    meshes.emplace_back(std::make_shared<mesh::MeshAsset>(std::move(new_mesh)));
    vertices.clear();
    indices.clear();
  }
  return meshes;
}
