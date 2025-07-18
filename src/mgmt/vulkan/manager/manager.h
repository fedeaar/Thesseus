#pragma once

#include "../buffer/buffer.h"
#include "../descriptors/descriptors.h"
#include "../image/image.h"
#include "../info/info.h"
#include "../manager.h"
#include "../pipeline/pipeline.h"

namespace mgmt {
namespace vulkan {

class Manager
{
public:
  core::status initialized = core::status::NOT_INIT;

private:
  DestructorQueue del_queue_{};
  mgmt::window::Manager* window_mgr_;

  // instance
  VkInstance instance_;
  VkDebugUtilsMessengerEXT debug_messenger_;
  // device
  VkPhysicalDevice gpu_;
  VkDevice device_;
  // alloc
  VmaAllocator allocator_;
  descriptor::StaticAllocator descriptor_allocator_;
  descriptor::DynamicAllocator global_dynamic_descriptor_allocator_;
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
  Manager(window::Manager* window_);

  core::code destroy();
  ~Manager();

  // get
  VkInstance const& get_instance();
  VkPhysicalDevice const& get_physical_dev();
  VkDevice const& get_dev();
  VkQueue const& get_graphics_queue();
  VmaAllocator const& get_allocator();
  VkSurfaceKHR const& get_surface();
  descriptor::DynamicAllocator& get_global_descriptor_allocator();
  u32 get_graphics_queue_family();

  // swapchain
  core::code create_swapchain(VkSurfaceFormatKHR format,
                              VkExtent2D& extent,
                              VkSwapchainKHR& swapchain,
                              std::vector<VkImage>& imgs,
                              std::vector<VkImageView>& imgs_views);
  core::code destroy_swapchain(VkSwapchainKHR& swapchain,
                               std::vector<VkImage>& imgs,
                               std::vector<VkImageView>& imgs_views);
  // buffers
  core::Result<buffer::AllocatedBuffer, core::code>
  create_buffer(size_t size, VkBufferUsageFlags flags, VmaMemoryUsage usage);
  core::code destroy_buffer(buffer::AllocatedBuffer const& buffer);

  // pipelines
  core::Result<pipeline::Pipeline, core::code> create_compute_pipeline(
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
  core::code create_image(VkExtent3D extent,
                          VkFormat format,
                          VkImageUsageFlags usage,
                          bool mipmapped,
                          image::AllocatedImage& image);
  core::code create_image(void* data,
                          VkExtent3D extent,
                          VkFormat format,
                          VkImageUsageFlags usage,
                          bool mipmapped,
                          image::AllocatedImage& image);
  core::code destroy_image(const image::AllocatedImage& img);

  // sync
  core::code create_fence(VkFenceCreateInfo& info, VkFence& fence);
  core::code create_semaphore(VkSemaphoreCreateInfo& info,
                              VkSemaphore& semaphore);
  core::code await(u32 count, VkFence* fences, bool wait_all, u32 timeout);

  // command
  core::code create_command_buffers(VkCommandPoolCreateInfo& info,
                                    VkCommandPool& pool,
                                    VkCommandBuffer& buffers);

  // descriptors
  core::Result<VkDescriptorPool, core::code> create_descriptor_pool(
    VkDescriptorPoolCreateInfo pool_info);
  core::code destroy_descriptor_pool(VkDescriptorPool& pool);
  core::code create_descriptors(
    std::span<descriptor::PoolSizeRatio> const& init_ratios,
    u32 set_count,
    descriptor::DynamicAllocator& descriptor);
  core::code create_descriptor_set(descriptor::LayoutBuilder& builder,
                                   VkShaderStageFlags flags,
                                   VkDescriptorSetLayout& layout,
                                   VkDescriptorSet& descriptor_set);
  core::code update_descriptor_set(descriptor::Writer& writer,
                                   VkDescriptorSet& descriptor_set);

  // dev
  core::code device_wait_idle();

private:
  core::code init_instance(vkb::Instance& vkb_instance);
  core::code init_surface();
  core::code init_devices(vkb::Instance const& vkb_instance,
                          vkb::Device& vkb_dev);
  core::code init_graphics_queue(vkb::Device const& vkb_dev);
  core::code init_allocator();
  core::code init_descriptors();
  core::code init_imm_submit();
};

} // namespace vulkan
} // namespace mgmt
