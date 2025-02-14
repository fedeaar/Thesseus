#pragma once

#include "../descriptors/descriptors.h"
#include "../image/image.h"
#include "../manager.h"
#include "../manager/manager.h"

namespace mgmt {
namespace vulkan {

class Swapchain
{
public:
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

  core::status initialized = core::status::NOT_INIT;

private:
  DestructorQueue del_queue_{};

  VkExtent2D extent;
  VkSurfaceFormatKHR surface_fmt;
  VkSwapchainKHR swapchain;
  std::vector<VkImage> imgs;
  std::vector<VkImageView> imgs_views;

  VkExtent2D draw_extent;
  image::AllocatedImage draw_img;
  image::AllocatedImage depth_img;

  VkDescriptorSet draw_img_descriptors;
  VkDescriptorSetLayout draw_img_descriptor_layout;

  Frame frames[FRAME_OVERLAP];

  u32 frame = 0;
  u32 current_img_idx = 0;
  f32 render_scale = 1.f;

public:
  core::code init(vulkan::Manager& vk_mgr);
  Swapchain();

  core::code destroy();
  ~Swapchain();

  Frame& get_current_frame();
  VkImage& get_current_image();
  VkImageView& get_current_image_view();
  VkCommandBuffer& get_current_cmd_buffer();

  core::code draw_img_transition(VkImageLayout current, VkImageLayout next);
  core::code depth_img_transition(VkImageLayout current, VkImageLayout next);
  core::code current_img_transition(VkImageLayout current, VkImageLayout next);

private:
  core::code create_frames(vulkan::Manager& vk_mgr);
};

} // namespace vulkan
} // namespace mgmt
