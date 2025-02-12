#pragma once

#include "../descriptors/descriptors.h"
#include "../image/image.h"
#include "../manager.h"

namespace mgmt {
namespace vulkan {

namespace swapchain {

static const u32 FRAME_OVERLAP = 2;

struct Frame
{
  VkCommandPool command_pool;
  VkCommandBuffer main_command_buffer;
  VkSemaphore swapchain_semaphore, render_semaphore;
  VkFence render_fence;
  DestructorQueue del_queue;
  descriptor::DynamicAllocator frame_descriptors;
};

struct Swapchain
{
  VkExtent2D extent;
  VkFormat image_fmt;
  VkSwapchainKHR swapchain;
  std::vector<VkImage> imgs;
  std::vector<VkImageView> img_views;
  image::AllocatedImage draw_img;
  image::AllocatedImage depth_img;
  VkExtent2D draw_extent;
  VkDescriptorSet draw_img_descriptors;
  VkDescriptorSetLayout draw_img_descriptor_layout;
  Frame frames[FRAME_OVERLAP];
  u32 frame = 0;

  Frame& get_frame(u32 frame_number);
};

} // namespace swapchain

} // namespace vulkan
} // namespace mgmt
