#include "manager.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

core::Result<ResourceManagement::VulkanManager::Swapchain::Swapchain,
             ResourceManagement::Status>
ResourceManagement::VulkanManager::Manager::create_swapchain()
{
  if (!initialized) {
    logger.error("create_swapchain failed, called before initialization");
    return ResourceManagement::Status::NOT_INIT;
  }
  Swapchain::Swapchain swapchain = { .image_fmt = VK_FORMAT_B8G8R8A8_UNORM };
  // we assume window_mgr is initialized
  u32 width = window_mgr_->get_extent().width;
  u32 height = window_mgr_->get_extent().height;
  u32 supported = -1;
  // auto supported_result = check(vkGetPhysicalDeviceSurfaceSupportKHR(
  //   gpu_, graphics_queue_family_, surface_, &supported));
  // if (supported_result != ResourceManagement::Status::SUCCESS || !supported)
  // {
  //   logger.error(fmt::format("create_swapchain failed, "
  //                            "vkGetPhysicalDeviceSurfaceSupportKHR "
  //                            "failed.\nsupported = {}",
  //                            supported));
  //   return ResourceManagement::Status::ERROR;
  // }
  vkb::SwapchainBuilder swapchain_builder{ gpu_, device_, surface_ };
  // create swapchain
  auto vkb_swapchain_result =
    swapchain_builder // .use_default_format_selection()
      .set_desired_format(
        VkSurfaceFormatKHR{ .format = swapchain.image_fmt,
                            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
      .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
      .set_desired_extent(width, height)
      .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
      .build();
  if (!vkb_swapchain_result.has_value()) {
    logger.error(
      fmt::format("create_swapchain failed, swapchain builder error: {}",
                  vkb_swapchain_result.error().message()));
    return ResourceManagement::Status::ERROR;
  }
  auto vkb_swapchain = vkb_swapchain_result.value();
  swapchain.extent = vkb_swapchain.extent;
  swapchain.swapchain = vkb_swapchain.swapchain;
  // create image
  auto vkb_swapchain_imgs_result = vkb_swapchain.get_images();
  if (!vkb_swapchain_imgs_result.has_value()) {
    logger.error(fmt::format("create_swapchain failed, get_images error: {}",
                             vkb_swapchain_imgs_result.error().message()));
    return ResourceManagement::Status::ERROR;
  }
  swapchain.imgs = vkb_swapchain_imgs_result.value();
  // create image views
  auto vkb_swapchain_img_views_result = vkb_swapchain.get_image_views();
  if (!vkb_swapchain_img_views_result.has_value()) {
    logger.error(
      fmt::format("create_swapchain failed, get_image_views error: {}",
                  vkb_swapchain_img_views_result.error().message()));
    return ResourceManagement::Status::ERROR;
  }
  swapchain.img_views = vkb_swapchain_img_views_result.value();
  // create draw image
  VkExtent3D draw_img_extent = { width, height, 1 };
  swapchain.draw_img.format = VK_FORMAT_R16G16B16A16_SFLOAT;
  swapchain.draw_img.extent = draw_img_extent;
  VkImageUsageFlags draw_img_usage{};
  draw_img_usage |=
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
    VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  VkImageCreateInfo draw_img_info =
    ResourceManagement::VulkanManager::Info::image_create_info(
      swapchain.draw_img.format, draw_img_usage, draw_img_extent);
  VmaAllocationCreateInfo draw_img_alloc_info = {};
  draw_img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  draw_img_alloc_info.requiredFlags =
    VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vmaCreateImage(allocator_,
                 &draw_img_info,
                 &draw_img_alloc_info,
                 &swapchain.draw_img.image,
                 &swapchain.draw_img.allocation,
                 nullptr);
  VkImageViewCreateInfo draw_img_view_info =
    ResourceManagement::VulkanManager::Info::imageview_create_info(
      swapchain.draw_img.format,
      swapchain.draw_img.image,
      VK_IMAGE_ASPECT_COLOR_BIT);
  auto status = check(vkCreateImageView(
    device_, &draw_img_view_info, nullptr, &swapchain.draw_img.view));
  if (status != ResourceManagement::Status::SUCCESS) {
    logger.error("create_swapchain failed, vkCreateImageView error");
    return status;
  }
  // create commands and sync structures
  auto fence_info = ResourceManagement::VulkanManager::Info::fence_create_info(
    VK_FENCE_CREATE_SIGNALED_BIT);
  auto semaphore_info =
    ResourceManagement::VulkanManager::Info::semaphore_create_info(0);
  auto command_pool_info =
    ResourceManagement::VulkanManager::Info::command_pool_create_info(
      graphics_queue_family_, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  for (int i = 0; i < Swapchain::FRAME_OVERLAP; i++) {
    status = check(vkCreateCommandPool(
      device_, &command_pool_info, nullptr, &swapchain.frames[i].command_pool));
    if (status != ResourceManagement::Status::SUCCESS) {
      logger.error("create_swapchain failed, vkCreateCommandPool error");
      return status;
    }
    auto command_buffer_alloc_info =
      ResourceManagement::VulkanManager::Info::command_buffer_allocate_info(
        swapchain.frames[i].command_pool, 1);
    status =
      check(vkAllocateCommandBuffers(device_,
                                     &command_buffer_alloc_info,
                                     &swapchain.frames[i].main_command_buffer));
    if (status != ResourceManagement::Status::SUCCESS) {
      logger.error("create_swapchain failed, vkAllocateCommandBuffers error");
      return status;
    }
    status = check(vkCreateFence(
      device_, &fence_info, nullptr, &swapchain.frames[i].render_fence));
    if (status != ResourceManagement::Status::SUCCESS) {
      logger.error("create_swapchain failed, vkCreateFence error");
      return status;
    }
    status = check(vkCreateSemaphore(device_,
                                     &semaphore_info,
                                     nullptr,
                                     &swapchain.frames[i].swapchain_semaphore));
    if (status != ResourceManagement::Status::SUCCESS) {
      logger.error("create_swapchain failed, vkCreateSemaphore error");
      return status;
    }
    status = check(vkCreateSemaphore(device_,
                                     &semaphore_info,
                                     nullptr,
                                     &swapchain.frames[i].render_semaphore));
    if (status != ResourceManagement::Status::SUCCESS) {
      logger.error("create_swapchain failed, vkCreateSemaphore error");
      return status;
    }
  }
  // build draw_img layout
  ResourceManagement::VulkanManager::Descriptor::LayoutBuilder layout_builder;
  layout_builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
  auto layout_result =
    layout_builder.build(device_, VK_SHADER_STAGE_COMPUTE_BIT);
  if (!layout_result.has_value()) {
    logger.error("create_swapchain failed, layout_builder error");
    return layout_result.error();
  }
  swapchain.draw_img_descriptor_layout = layout_result.value();
  // allocate a descriptor set
  auto descriptor_result = descriptor_allocator_.allocate(
    device_, swapchain.draw_img_descriptor_layout);
  if (!descriptor_result.has_value()) {
    logger.error("create_swapchain failed, descriptor_allocator error");
    return descriptor_result.error();
  }
  swapchain.draw_img_descriptors = descriptor_result.value();
  // update descriptor sets
  VkDescriptorImageInfo img_info_{};
  img_info_.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
  img_info_.imageView = swapchain.draw_img.view;
  VkWriteDescriptorSet draw_img_write_ = {};
  draw_img_write_.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  draw_img_write_.pNext = nullptr;
  draw_img_write_.dstBinding = 0;
  draw_img_write_.dstSet = swapchain.draw_img_descriptors;
  draw_img_write_.descriptorCount = 1;
  draw_img_write_.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  draw_img_write_.pImageInfo = &img_info_;
  vkUpdateDescriptorSets(device_, 1, &draw_img_write_, 0, nullptr);
  // set destroyer
  del_queue_.push([=]() {
    logger.error("called swapchain destroy");
    vkDeviceWaitIdle(device_);
    vkDestroyDescriptorSetLayout(
      device_, swapchain.draw_img_descriptor_layout, nullptr);
    for (int i = 0; i < Swapchain::FRAME_OVERLAP; i++) {
      vkDestroyCommandPool(device_, swapchain.frames[i].command_pool, nullptr);
      vkDestroyFence(device_, swapchain.frames[i].render_fence, nullptr);
      vkDestroySemaphore(
        device_, swapchain.frames[i].render_semaphore, nullptr);
      vkDestroySemaphore(
        device_, swapchain.frames[i].swapchain_semaphore, nullptr);
      // swapchain.frames_[i].del_queue.flush();
    }
    vkDestroyImageView(device_, swapchain.draw_img.view, nullptr);
    vmaDestroyImage(
      allocator_, swapchain.draw_img.image, swapchain.draw_img.allocation);
    vkDestroySwapchainKHR(device_, swapchain.swapchain, nullptr);
    for (int i = 0; i < swapchain.img_views.size(); i++) {
      vkDestroyImageView(device_, swapchain.img_views[i], nullptr);
    }
  });
  // success
  return swapchain;
}
