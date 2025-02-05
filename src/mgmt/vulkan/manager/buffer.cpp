#include "manager.h"

//
// create
//

core::Result<mgmt::vulkan::buffer::AllocatedBuffer, core::Status>
mgmt::vulkan::Manager::create_buffer(size_t allocSize,
                                     VkBufferUsageFlags flags,
                                     VmaMemoryUsage usage)
{
  VkBufferCreateInfo buffer_info = { .sType =
                                       VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
  buffer_info.pNext = nullptr;
  buffer_info.size = allocSize;
  buffer_info.usage = usage;
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
  if (status != core::Status::SUCCESS) {
    logger.err("create_buffer failed, vmaCreateBuffer error");
    return core::Status::ERROR;
  }
  return buffer;
}

//
// destroy
//

core::Status
mgmt::vulkan::Manager::destroy_buffer(
  mgmt::vulkan::buffer::AllocatedBuffer const& buffer)
{
  vmaDestroyBuffer(allocator_, buffer.buffer, buffer.allocation);
  return core::Status::SUCCESS;
}

//
// mesh
//

core::Result<mgmt::vulkan::mesh::GPUMeshBuffers, core::Status>
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
    logger.err("upload_mesh failed, could not create vertex buffer");
    return core::Status::ERROR;
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
    logger.err("upload_mesh failed, could not create index buffer");
    return core::Status::ERROR;
  }
  mesh.index_buff = index_buff_result.value();
  auto staging_result = create_buffer(
    vb_s + ib_s, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
  if (!index_buff_result.has_value()) {
    logger.err("upload_mesh failed, could not create staging buffer");
    return core::Status::ERROR;
  }
  auto staging = staging_result.value();
  void* data;
  vmaMapMemory(allocator_, staging.allocation, &data);
  memcpy(data, vertices.data(), vb_s);
  memcpy((char*)data + vb_s, indices.data(), ib_s);
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
