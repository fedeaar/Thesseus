#pragma once

#include "../descriptors/descriptors.h"
#include "../info/info.h"
#include "../manager.h"
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
  Descriptor::Allocator descriptor_allocator_;
  // graphics queue
  VkQueue graphics_queue_;
  u32 graphics_queue_family_;
  // surface
  VkSurfaceKHR surface_;

public:
  core::Status init();
  Manager(mgmt::WindowManager* window_);

  core::Status destroy();
  ~Manager();

  VkInstance const& get_instance();
  VkPhysicalDevice const& get_physical_dev();
  VkDevice const& get_dev();
  VkQueue const& get_graphics_queue();

  core::Result<Swapchain::Swapchain, core::Status> create_swapchain();
  core::Result<Pipeline::Pipeline, core::Status> create_pipeline(
    Swapchain::Swapchain& swapchain,
    VkPipelineLayoutCreateInfo& layout_info,
    char* shader_path);
  core::Result<VkDescriptorPool, core::Status> create_descriptor_pool(
    VkDescriptorPoolCreateInfo pool_info);

  core::Result<VkCommandBuffer, core::Status> swapchain_begin_commands(
    u32 frame_number,
    Swapchain::Swapchain& swapchain,
    u32& img_idx);
  core::Status swapchain_end_commands(VkCommandBuffer cmd,
                                      u32 frame_number,
                                      u32 img_idx,
                                      Swapchain::Swapchain& swapchain);
};

} // namespace vulkan
} // namespace mgmt
