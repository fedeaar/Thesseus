#pragma once

#include "../vulkan-manager.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>

namespace RenderEngine {
namespace VulkanManager {

class Manager
{
public:
  bool initialized = false;

private:
  DestructorQueue del_queue_{};

  Engine* engine_;
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
  Manager(Engine* engine_);

  Status destroy();
  ~Manager();

  core::Result<Swapchain, Status> create_swapchain();
  // core::Result<ImmediateSubmit, Status> create_immediate_submit();
  core::Result<Pipeline::Pipeline, Status> create_pipeline(
    Swapchain& swapchain,
    VkPipelineLayoutCreateInfo& layout_info,
    char* shader_path);
  core::Result<VkDescriptorPool, Status> create_descriptor_pool(
    VkDescriptorPoolCreateInfo pool_info);
};

} // namespace VulkanManager
} // namespace RenderEngine
