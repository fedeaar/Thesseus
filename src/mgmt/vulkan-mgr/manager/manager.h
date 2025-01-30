#pragma once

#include "../descriptors/descriptors.h"
#include "../info/info.h"
#include "../pipeline/pipeline.h"
#include "../swapchain/swapchain.h"
#include "../vulkan-manager.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace ResourceManagement {
namespace VulkanManager {

class Manager
{
public:
  bool initialized = false;

private:
  ResourceManagement::WindowManager* window_mgr_;
  DestructorQueue del_queue_{};

  // instance
  VkInstance instance_;
  VkDebugUtilsMessengerEXT debug_messenger_;
  // device
  VkPhysicalDevice gpu_;
  VkDevice device_;
  // alloc
  VmaAllocator allocator_;
  Descriptor::Allocator descriptor_allocator_;
  // graphics queue
  VkQueue graphics_queue_;
  u32 graphics_queue_family_;
  // surface
  VkSurfaceKHR surface_;

public:
  Status init();
  Manager(ResourceManagement::WindowManager* window_);

  Status destroy();
  ~Manager();

  VkInstance const& get_instance();
  VkPhysicalDevice const& get_physical_dev();
  VkDevice const& get_dev();
  VkQueue const& get_graphics_queue();

  core::Result<Swapchain::Swapchain, Status> create_swapchain();
  // core::Result<ImmediateSubmit, Status> create_immediate_submit();
  core::Result<Pipeline::Pipeline, Status> create_pipeline(
    Swapchain::Swapchain& swapchain,
    VkPipelineLayoutCreateInfo& layout_info,
    char* shader_path);
  core::Result<VkDescriptorPool, Status> create_descriptor_pool(
    VkDescriptorPoolCreateInfo pool_info);

  core::Result<VkCommandBuffer, Status> swapchain_begin_commands(
    u32 frame_number,
    Swapchain::Swapchain& swapchain,
    u32& img_idx);
  Status swapchain_end_commands(VkCommandBuffer cmd,
                                u32 frame_number,
                                u32 img_idx,
                                Swapchain::Swapchain& swapchain);
};

} // namespace VulkanManager
} // namespace ResourceManagement
