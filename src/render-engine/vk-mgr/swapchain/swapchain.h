#pragma once

#include "../vulkan-manager.h"

#include <vk_mem_alloc.h>

namespace RenderEngine {
namespace VulkanManager {

static const u32 SWAPCHAIN_FRAME_OVERLAP = 2;

class Swapchain
{
public:
  struct Frame
  {
    VkCommandPool command_pool;
    VkCommandBuffer main_command_buffer;
    VkSemaphore swapchain_semaphore, render_semaphore;
    VkFence render_fence;
    DestructorQueue del_queue;
  };

  struct AllocatedImage
  {
    VkImage image;
    VkImageView view;
    VkFormat format;
    VkExtent3D extent;
    VmaAllocation allocation;
  };
  struct Params
  {
    VkExtent2D swapchain_extent;
    VkFormat swapchain_image_fmt = VK_FORMAT_B8G8R8A8_UNORM;
    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapchain_imgs;
    std::vector<VkImageView> swapchain_img_views;
    AllocatedImage draw_img;
    VkExtent2D draw_extent;
    Frame frames[SWAPCHAIN_FRAME_OVERLAP];
    // descriptors
    VkDescriptorSetLayout draw_img_descriptor_layout;
    VkDescriptorSet draw_img_descriptors;
  };

private:
  Manager* vk_mgr_;
  Params params_;
  u64 frame_ = 0;

public:
  Swapchain(Manager* manager, Params const& params);
  ~Swapchain();

  Frame const& get_current_frame();

  Status await_render();
};

} // namespace VulkanManager
} // namespace RenderEngine
