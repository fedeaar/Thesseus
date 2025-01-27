#include "manager.h"

using namespace RenderEngine;
using namespace RenderEngine::VulkanManager;

core::Result<Swapchain, Status>
Manager::create_swapchain()
{
  // todo@engine: separate and handle in swapchain
  if (!initialized) {
    logger.error("create_swapchain failed, called before initialization");
    return Status::NOT_INIT;
  }
  Swapchain::Params params = {};
  auto engine = *engine_;
  // we assume window_mgr is initialized
  auto window_mgr = engine.get_window_mgr().value();
  u32 width = window_mgr.width;
  u32 height = window_mgr.height;
  vkb::SwapchainBuilder swapchain_builder{ gpu_, device_, surface_ };
  // create swapchain
  auto vkb_swapchain_result =
    swapchain_builder.use_default_format_selection()
      .set_desired_format(
        VkSurfaceFormatKHR{ .format = params.swapchain_image_fmt,
                            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
      .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
      .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
      .set_desired_extent(width, height)
      .build();
  if (!vkb_swapchain_result.has_value()) {
    logger.error(
      fmt::format("create_swapchain failed, swapchain builder error: {}",
                  vkb_swapchain_result.full_error()));
    return Status::ERROR;
  }
  auto vkb_swapchain = vkb_swapchain_result.value();
  params.swapchain = vkb_swapchain.swapchain;
  params.swapchain_extent = vkb_swapchain.extent;
  // create image
  auto vkb_swapchain_imgs_result = vkb_swapchain.get_images();
  if (!vkb_swapchain_imgs_result.has_value()) {
    logger.error(fmt::format("create_swapchain failed, get_images error: {}",
                             vkb_swapchain_imgs_result.full_error()));
    return Status::ERROR;
  }
  params.swapchain_imgs = vkb_swapchain_imgs_result.value();
  // create image views
  auto vkb_swapchain_img_views_result = vkb_swapchain.get_image_views();
  if (!vkb_swapchain_img_views_result.has_value()) {
    logger.error(
      fmt::format("create_swapchain failed, get_image_views error: {}",
                  vkb_swapchain_img_views_result.full_error()));
    return Status::ERROR;
  }
  params.swapchain_img_views = vkb_swapchain_img_views_result.value();
  // create draw image
  VkExtent3D draw_img_extent = { width, height, 1 };
  params.draw_img.format = VK_FORMAT_R16G16B16A16_SFLOAT;
  params.draw_img.extent = draw_img_extent;
  VkImageUsageFlags draw_img_usage{};
  draw_img_usage |=
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
    VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  VkImageCreateInfo draw_img_info = Info::image_create_info(
    params.draw_img.format, draw_img_usage, draw_img_extent);
  VmaAllocationCreateInfo draw_img_alloc_info = {};
  draw_img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  draw_img_alloc_info.requiredFlags =
    VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vmaCreateImage(allocator_,
                 &draw_img_info,
                 &draw_img_alloc_info,
                 &params.draw_img.image,
                 &params.draw_img.allocation,
                 nullptr);
  VkImageViewCreateInfo draw_img_view_info = Info::imageview_create_info(
    params.draw_img.format, params.draw_img.image, VK_IMAGE_ASPECT_COLOR_BIT);
  auto status = check(vkCreateImageView(
    device_, &draw_img_view_info, nullptr, &params.draw_img.view));
  if (status != Status::SUCCESS) {
    logger.error("create_swapchain failed, vkCreateImageView error");
    return status;
  }
  // create commands and sync structures
  auto fence_info = Info::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
  auto semaphore_info = Info::semaphore_create_info(0);
  auto command_pool_info = Info::command_pool_create_info(
    graphics_queue_family_, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  for (int i = 0; i < SWAPCHAIN_FRAME_OVERLAP; i++) {
    status = check(vkCreateCommandPool(
      device_, &command_pool_info, nullptr, &params.frames[i].command_pool));
    if (status != Status::SUCCESS) {
      logger.error("create_swapchain failed, vkCreateCommandPool error");
      return status;
    }
    auto command_buffer_alloc_info =
      Info::command_buffer_allocate_info(params.frames[i].command_pool, 1);
    status =
      check(vkAllocateCommandBuffers(device_,
                                     &command_buffer_alloc_info,
                                     &params.frames[i].main_command_buffer));
    if (status != Status::SUCCESS) {
      logger.error("create_swapchain failed, vkAllocateCommandBuffers error");
      return status;
    }
    status = check(vkCreateFence(
      device_, &fence_info, nullptr, &params.frames[i].render_fence));
    if (status != Status::SUCCESS) {
      logger.error("create_swapchain failed, vkCreateFence error");
      return status;
    }
    status = check(vkCreateSemaphore(device_,
                                     &semaphore_info,
                                     nullptr,
                                     &params.frames[i].swapchain_semaphore));
    if (status != Status::SUCCESS) {
      logger.error("create_swapchain failed, vkCreateSemaphore error");
      return status;
    }
    status = check(vkCreateSemaphore(
      device_, &semaphore_info, nullptr, &params.frames[i].render_semaphore));
    if (status != Status::SUCCESS) {
      logger.error("create_swapchain failed, vkCreateSemaphore error");
      return status;
    }
  }
  // build draw_img layout
  Descriptor::LayoutBuilder layout_builder;
  layout_builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
  auto layout_result =
    layout_builder.build(device_, VK_SHADER_STAGE_COMPUTE_BIT);
  if (!layout_result.has_value()) {
    logger.error("create_swapchain failed, layout_builder error");
    return layout_result.error();
  }
  params.draw_img_descriptor_layout = layout_result.value();
  // allocate a descriptor set
  auto descriptor_result =
    descriptor_allocator_.allocate(device_, params.draw_img_descriptor_layout);
  if (!descriptor_result.has_value()) {
    logger.error("create_swapchain failed, descriptor_allocator error");
    return descriptor_result.error();
  }
  params.draw_img_descriptors = descriptor_result.value();
  // update descriptor sets
  VkDescriptorImageInfo img_info_{};
  img_info_.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
  img_info_.imageView = params.draw_img.view;
  VkWriteDescriptorSet draw_img_write_ = {};
  draw_img_write_.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  draw_img_write_.pNext = nullptr;
  draw_img_write_.dstBinding = 0;
  draw_img_write_.dstSet = params.draw_img_descriptors;
  draw_img_write_.descriptorCount = 1;
  draw_img_write_.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  draw_img_write_.pImageInfo = &img_info_;
  vkUpdateDescriptorSets(device_, 1, &draw_img_write_, 0, nullptr);
  // set destroyer
  del_queue_.push([=]() {
    vkDeviceWaitIdle(device_);
    vkDestroyDescriptorSetLayout(
      device_, params.draw_img_descriptor_layout, nullptr);
    for (int i = 0; i < SWAPCHAIN_FRAME_OVERLAP; i++) {
      vkDestroyCommandPool(device_, params.frames[i].command_pool, nullptr);
      vkDestroyFence(device_, params.frames[i].render_fence, nullptr);
      vkDestroySemaphore(device_, params.frames[i].render_semaphore, nullptr);
      vkDestroySemaphore(
        device_, params.frames[i].swapchain_semaphore, nullptr);
      // params.frames_[i].del_queue.flush();
    }
    vkDestroyImageView(device_, params.draw_img.view, nullptr);
    vmaDestroyImage(
      allocator_, params.draw_img.image, params.draw_img.allocation);
    vkDestroySwapchainKHR(device_, params.swapchain, nullptr);
    for (int i = 0; i < params.swapchain_img_views.size(); i++) {
      vkDestroyImageView(device_, params.swapchain_img_views[i], nullptr);
    }
  });
  // success
  return Swapchain(this, params);
}
