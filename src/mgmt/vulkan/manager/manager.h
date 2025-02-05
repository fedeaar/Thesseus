#pragma once

#include "../buffer/buffer.h"
#include "../descriptors/descriptors.h"
#include "../info/info.h"
#include "../manager.h"
#include "../mesh/mesh.h"
#include "../pipeline/pipeline.h"
#include "../swapchain/swapchain.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace mgmt {
namespace vulkan {

class Manager
{
public:
  bool initialized = false;

private:
  mgmt::WindowManager* window_mgr_;
  DestructorQueue del_queue_{};

  // instance
  VkInstance instance_;
  VkDebugUtilsMessengerEXT debug_messenger_;
  // device
  VkPhysicalDevice gpu_;
  VkDevice device_;
  // alloc
  VmaAllocator allocator_;
  descriptor::Allocator descriptor_allocator_;
  // graphics queue
  VkQueue graphics_queue_;
  u32 graphics_queue_family_;
  // surface
  VkSurfaceKHR surface_;
  // imm submit
  struct
  {
    VkFence fence;
    VkCommandBuffer command_buffer;
    VkCommandPool command_pool;
  } imm_submit_;

public:
  core::Status init();
  Manager(mgmt::WindowManager* window_);

  core::Status destroy();
  ~Manager();

  VkInstance const& get_instance();
  VkPhysicalDevice const& get_physical_dev();
  VkDevice const& get_dev();
  VkQueue const& get_graphics_queue();

  // descriptors
  core::Result<VkDescriptorPool, core::Status> create_descriptor_pool(
    VkDescriptorPoolCreateInfo pool_info);
  // buffers
  core::Result<buffer::AllocatedBuffer, core::Status>
  create_buffer(size_t size, VkBufferUsageFlags flags, VmaMemoryUsage usage);
  core::Status destroy_buffer(buffer::AllocatedBuffer const& buffer);
  core::Result<mesh::GPUMeshBuffers, core::Status> upload_mesh(
    std::span<u32> indices,
    std::span<mesh::Vertex> vertices);
  core::Result<std::vector<std::shared_ptr<mesh::MeshAsset>>, core::Status>
  load_gltf_meshes(char* path);
  // swapchain
  core::Result<swapchain::Swapchain, core::Status> create_swapchain();
  core::Result<VkCommandBuffer, core::Status> swapchain_begin_commands(
    u32 frame_number,
    swapchain::Swapchain& swapchain,
    u32& img_idx);
  core::Status swapchain_end_commands(VkCommandBuffer cmd,
                                      u32 frame_number,
                                      u32 img_idx,
                                      swapchain::Swapchain& swapchain);
  // pipelines
  core::Result<pipeline::Pipeline, core::Status> create_compute_pipeline(
    swapchain::Swapchain& swapchain,
    VkPipelineLayoutCreateInfo& layout_info,
    char* shader_path);
  core::Result<pipeline::Pipeline, core::Status> create_gfx_pipeline(
    VkPipelineLayoutCreateInfo& layout_info,
    pipeline::Builder builder,
    char* vs_path,
    char* fs_path);
  // imm submit
  core::Status imm_submit(std::function<void(VkCommandBuffer cmd)>&& function);
};

} // namespace vulkan
} // namespace mgmt
