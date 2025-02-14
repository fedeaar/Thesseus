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
  core::status initialized = core::status::NOT_INIT;
  bool resize_requested = false; // TODO: move to swapchain

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
  descriptor::StaticAllocator descriptor_allocator_;
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
  core::code init();
  Manager(mgmt::WindowManager* window_);

  core::code destroy();
  ~Manager();

  VkInstance const& get_instance();
  VkPhysicalDevice const& get_physical_dev();
  VkDevice const& get_dev();
  VkQueue const& get_graphics_queue();
  VmaAllocator const& get_allocator();

  // descriptors
  core::Result<VkDescriptorPool, core::code> create_descriptor_pool(
    VkDescriptorPoolCreateInfo pool_info);
  core::code destroy_descriptor_pool(VkDescriptorPool& pool);
  // buffers
  core::Result<buffer::AllocatedBuffer, core::code>
  create_buffer(size_t size, VkBufferUsageFlags flags, VmaMemoryUsage usage);
  core::code destroy_buffer(buffer::AllocatedBuffer const& buffer);
  core::Result<mesh::GPUMeshBuffers, core::code> upload_mesh(
    std::span<u32> indices,
    std::span<mesh::Vertex> vertices);
  core::Result<std::vector<std::shared_ptr<mesh::MeshAsset>>, core::code>
  load_gltf_meshes(char* path);
  // swapchain
  core::Result<swapchain::Swapchain, core::code> create_swapchain();
  core::code _destroy_swapchain(swapchain::Swapchain& swapchain);
  core::code resize_swapchain(swapchain::Swapchain& swapchain);
  core::code swapchain_begin_commands(swapchain::Swapchain& swapchain);
  core::code swapchain_end_commands(swapchain::Swapchain& swapchain);
  // pipelines
  core::Result<pipeline::Pipeline, core::code> create_compute_pipeline(
    swapchain::Swapchain& swapchain,
    VkPipelineLayoutCreateInfo& layout_info,
    char* shader_path);
  core::Result<pipeline::Pipeline, core::code> create_gfx_pipeline(
    VkPipelineLayoutCreateInfo& layout_info,
    pipeline::Builder builder,
    char* vs_path,
    char* fs_path);
  // imm submit
  core::code imm_submit(std::function<void(VkCommandBuffer cmd)>&& function);
  // images
  core::Result<image::AllocatedImage, core::code> create_image(
    VkExtent3D size,
    VkFormat format,
    VkImageUsageFlags usage,
    bool mipmapped = false);
  core::Result<image::AllocatedImage, core::code> create_image(
    void* data,
    VkExtent3D size,
    VkFormat format,
    VkImageUsageFlags usage,
    bool mipmapped = false);
  core::code destroy_image(const image::AllocatedImage& img);
  // dev
  core::code device_wait_idle();
};

} // namespace vulkan
} // namespace mgmt
