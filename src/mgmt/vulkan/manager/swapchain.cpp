#include "manager.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

//
// create
//

core::Result<mgmt::vulkan::swapchain::Swapchain, core::Status>
mgmt::vulkan::Manager::create_swapchain()
{
  if (!initialized) {
    logger.err("create_swapchain failed, called before initialization");
    return core::Status::NOT_INIT;
  }
  swapchain::Swapchain swapchain = { .image_fmt = VK_FORMAT_B8G8R8A8_UNORM };
  // we assume window_mgr is initialized
  u32 width = window_mgr_->get_extent().width;
  u32 height = window_mgr_->get_extent().height;
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
    logger.err("create_swapchain failed, swapchain builder error: {}",
               vkb_swapchain_result.error().message());
    return core::Status::ERROR;
  }
  auto vkb_swapchain = vkb_swapchain_result.value();
  swapchain.extent = vkb_swapchain.extent;
  swapchain.swapchain = vkb_swapchain.swapchain;
  // create image
  auto vkb_swapchain_imgs_result = vkb_swapchain.get_images();
  if (!vkb_swapchain_imgs_result.has_value()) {
    logger.err("create_swapchain failed, get_images error: {}",
               vkb_swapchain_imgs_result.error().message());
    return core::Status::ERROR;
  }
  swapchain.imgs = vkb_swapchain_imgs_result.value();
  // create image views
  auto vkb_swapchain_img_views_result = vkb_swapchain.get_image_views();
  if (!vkb_swapchain_img_views_result.has_value()) {
    logger.err("create_swapchain failed, get_image_views error: {}",
               vkb_swapchain_img_views_result.error().message());
    return core::Status::ERROR;
  }
  swapchain.img_views = vkb_swapchain_img_views_result.value();
  // create draw image
  VkExtent3D draw_img_extent = { width, height, 1 };
  swapchain.draw_img.format = VK_FORMAT_R16G16B16A16_SFLOAT;
  swapchain.draw_img.extent = draw_img_extent;
  swapchain.draw_extent = { width, height };
  VkImageUsageFlags draw_img_usage{};
  draw_img_usage |=
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
    VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  VkImageCreateInfo draw_img_info = mgmt::vulkan::info::image_create_info(
    swapchain.draw_img.format, draw_img_usage, draw_img_extent);
  VmaAllocationCreateInfo draw_img_alloc_info = {};
  draw_img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  draw_img_alloc_info.requiredFlags =
    VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  auto status = check(vmaCreateImage(allocator_,
                                     &draw_img_info,
                                     &draw_img_alloc_info,
                                     &swapchain.draw_img.image,
                                     &swapchain.draw_img.allocation,
                                     nullptr));
  if (status != core::Status::SUCCESS) {
    logger.err("create_swapchain failed, could not create draw img view");
    return core::Status::ERROR;
  }
  VkImageViewCreateInfo draw_img_view_info =
    mgmt::vulkan::info::imageview_create_info(swapchain.draw_img.format,
                                              swapchain.draw_img.image,
                                              VK_IMAGE_ASPECT_COLOR_BIT);
  status = check(vkCreateImageView(
    device_, &draw_img_view_info, nullptr, &swapchain.draw_img.view));
  if (status != core::Status::SUCCESS) {
    logger.err("create_swapchain failed, vkCreateImageView error");
    return status;
  }
  // create depth img
  swapchain.depth_img.format = VK_FORMAT_D32_SFLOAT;
  swapchain.depth_img.extent = draw_img_extent;
  VkImageUsageFlags depth_img_usage{};
  depth_img_usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  VkImageCreateInfo depth_img_info = info::image_create_info(
    swapchain.depth_img.format, depth_img_usage, draw_img_extent);
  status = check(vmaCreateImage(allocator_,
                                &depth_img_info,
                                &draw_img_alloc_info,
                                &swapchain.depth_img.image,
                                &swapchain.depth_img.allocation,
                                nullptr));
  if (status != core::Status::SUCCESS) {
    logger.err("create_swapchain failed, could not create depth img");
    return core::Status::ERROR;
  }
  VkImageViewCreateInfo depth_view_info =
    info::imageview_create_info(swapchain.depth_img.format,
                                swapchain.depth_img.image,
                                VK_IMAGE_ASPECT_DEPTH_BIT);
  status = check(vkCreateImageView(
    device_, &depth_view_info, nullptr, &swapchain.depth_img.view));
  if (status != core::Status::SUCCESS) {
    logger.err("create_swapchain failed, could not create depth img view");
    return core::Status::ERROR;
  }
  // create commands and sync structures
  auto fence_info =
    mgmt::vulkan::info::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
  auto semaphore_info = mgmt::vulkan::info::semaphore_create_info(0);
  auto command_pool_info = mgmt::vulkan::info::command_pool_create_info(
    graphics_queue_family_, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  for (int i = 0; i < swapchain::FRAME_OVERLAP; i++) {
    std::vector<descriptor::PoolSizeRatio> frame_sizes = {
      { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
      { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
      { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
      { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 },
    };
    swapchain.frames[i].frame_descriptors = descriptor::DynamicAllocator{};
    swapchain.frames[i].frame_descriptors.init(device_, 1000, frame_sizes);
    status = check(vkCreateCommandPool(
      device_, &command_pool_info, nullptr, &swapchain.frames[i].command_pool));
    if (status != core::Status::SUCCESS) {
      logger.err("create_swapchain failed, vkCreateCommandPool error");
      return status;
    }
    auto command_buffer_alloc_info =
      mgmt::vulkan::info::command_buffer_allocate_info(
        swapchain.frames[i].command_pool, 1);
    status =
      check(vkAllocateCommandBuffers(device_,
                                     &command_buffer_alloc_info,
                                     &swapchain.frames[i].main_command_buffer));
    if (status != core::Status::SUCCESS) {
      logger.err("create_swapchain failed, vkAllocateCommandBuffers error");
      return status;
    }
    status = check(vkCreateFence(
      device_, &fence_info, nullptr, &swapchain.frames[i].render_fence));
    if (status != core::Status::SUCCESS) {
      logger.err("create_swapchain failed, vkCreateFence error");
      return status;
    }
    status = check(vkCreateSemaphore(device_,
                                     &semaphore_info,
                                     nullptr,
                                     &swapchain.frames[i].swapchain_semaphore));
    if (status != core::Status::SUCCESS) {
      logger.err("create_swapchain failed, vkCreateSemaphore error");
      return status;
    }
    status = check(vkCreateSemaphore(device_,
                                     &semaphore_info,
                                     nullptr,
                                     &swapchain.frames[i].render_semaphore));
    if (status != core::Status::SUCCESS) {
      logger.err("create_swapchain failed, vkCreateSemaphore error");
      return status;
    }
  }
  // build draw_img layout
  mgmt::vulkan::descriptor::LayoutBuilder layout_builder;
  layout_builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
  auto layout_result =
    layout_builder.build(device_, VK_SHADER_STAGE_COMPUTE_BIT);
  if (!layout_result.has_value()) {
    logger.err("create_swapchain failed, layout_builder error");
    return layout_result.error();
  }
  swapchain.draw_img_descriptor_layout = layout_result.value();
  // allocate a descriptor set
  auto descriptor_result = descriptor_allocator_.allocate(
    device_, swapchain.draw_img_descriptor_layout);
  if (!descriptor_result.has_value()) {
    logger.err("create_swapchain failed, descriptor_allocator error");
    return descriptor_result.error();
  }
  swapchain.draw_img_descriptors = descriptor_result.value();
  // update descriptor sets
  descriptor::Writer writer;
  writer.write_image(0,
                     swapchain.draw_img.view,
                     VK_NULL_HANDLE,
                     VK_IMAGE_LAYOUT_GENERAL,
                     VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
  writer.update_set(device_, swapchain.draw_img_descriptors);
  // set destroyer
  del_queue_.push(
    [=]() mutable { // TODO@engine: no need to keep swapchain existing here ?
      vkDeviceWaitIdle(device_);
      vkDestroyDescriptorSetLayout(
        device_, swapchain.draw_img_descriptor_layout, nullptr);
      for (int i = 0; i < swapchain::FRAME_OVERLAP; i++) {
        vkDestroyCommandPool(
          device_, swapchain.frames[i].command_pool, nullptr);
        vkDestroyFence(device_, swapchain.frames[i].render_fence, nullptr);
        vkDestroySemaphore(
          device_, swapchain.frames[i].render_semaphore, nullptr);
        vkDestroySemaphore(
          device_, swapchain.frames[i].swapchain_semaphore, nullptr);
        swapchain.frames[i].frame_descriptors.destroy_pools(device_);
        swapchain.frames[i].del_queue.flush();
      }
      vkDestroyImageView(device_, swapchain.depth_img.view, nullptr);
      vmaDestroyImage(
        allocator_, swapchain.depth_img.image, swapchain.depth_img.allocation);
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

core::Status
mgmt::vulkan::Manager::_destroy_swapchain(swapchain::Swapchain& swapchain)
{
  vkDestroySwapchainKHR(device_, swapchain.swapchain, nullptr);
  // destroy swapchain resources
  for (int i = 0; i < swapchain.img_views.size(); ++i) {
    vkDestroyImageView(device_, swapchain.img_views[i], nullptr);
  }
}

//
// commands
//

core::Result<VkCommandBuffer, core::Status>
mgmt::vulkan::Manager::swapchain_begin_commands(u32 frame_number,
                                                swapchain::Swapchain& swapchain,
                                                u32& img_idx)
{
  // we assume we are init
  auto current_frame = swapchain.get_frame(frame_number);
  // wait and prepare for next render
  auto status = check(vkWaitForFences(device_,
                                      1,
                                      &current_frame.render_fence,
                                      true,
                                      1000000000)); // 1s
  if (status != core::Status::SUCCESS) {
    logger.log("ready_swapchain failed awaiting render fence");
    return core::Status::ERROR;
  }
  current_frame.del_queue.flush();
  current_frame.frame_descriptors.clear_pools(device_);
  auto vk_err = vkAcquireNextImageKHR(device_,
                                      swapchain.swapchain,
                                      1000000000,
                                      current_frame.swapchain_semaphore,
                                      nullptr,
                                      &img_idx);
  if (vk_err == VK_ERROR_OUT_OF_DATE_KHR) {
    resize_requested = true;
    return core::Status::RETRYABLE_ERROR;
  } else if (vk_err && vk_err != VK_SUBOPTIMAL_KHR) {
    logger.log("ready_swapchain failed acquiring image");
    return core::Status::ERROR;
  }
  status = check(vkResetFences(device_, 1, &current_frame.render_fence));
  if (status != core::Status::SUCCESS) {
    logger.log("ready_swapchain failed resetting fence");
    return core::Status::ERROR;
  }
  VkCommandBuffer cmd = current_frame.main_command_buffer;
  status = check(vkResetCommandBuffer(cmd, 0));
  if (status != core::Status::SUCCESS) {
    logger.log("ready_swapchain failed resetting command buffer");
    return core::Status::ERROR;
  }
  VkCommandBufferBeginInfo cmd_info = info::command_buffer_begin_info(
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  status = check(vkBeginCommandBuffer(cmd, &cmd_info));
  if (status != core::Status::SUCCESS) {
    logger.log("ready_swapchain failed initializing command buffer");
    return core::Status::ERROR;
  }
  return cmd;
}

core::Status
mgmt::vulkan::Manager::swapchain_end_commands(VkCommandBuffer cmd,
                                              u32 frame_number,
                                              u32 img_idx,
                                              swapchain::Swapchain& swapchain)
{
  auto status = check(vkEndCommandBuffer(cmd));
  if (status != core::Status::SUCCESS) {
    return core::Status::ERROR; // todo@mgmt: log error msg
  }
  auto current_frame = swapchain.get_frame(frame_number);
  // prepare the submission to the queue
  VkCommandBufferSubmitInfo cmd_submit_info =
    info::command_buffer_submit_info(cmd);
  VkSemaphoreSubmitInfo wait_info = mgmt::vulkan::info::semaphore_submit_info(
    VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
    current_frame.swapchain_semaphore);
  VkSemaphoreSubmitInfo signal_info = mgmt::vulkan::info::semaphore_submit_info(
    VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, current_frame.render_semaphore);
  VkSubmitInfo2 submit =
    mgmt::vulkan::info::submit_info(&cmd_submit_info, &signal_info, &wait_info);
  // submit
  status = check(
    vkQueueSubmit2(graphics_queue_, 1, &submit, current_frame.render_fence));
  if (status != core::Status::SUCCESS) {
    return core::Status::ERROR; // todo@mgmt: log error msg
  }
  // display
  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.pNext = nullptr;
  present_info.pSwapchains = &swapchain.swapchain;
  present_info.swapchainCount = 1;
  present_info.pWaitSemaphores = &current_frame.render_semaphore;
  present_info.waitSemaphoreCount = 1;
  present_info.pImageIndices = &img_idx;
  auto vk_err = vkQueuePresentKHR(graphics_queue_, &present_info);
  if (vk_err == VK_ERROR_OUT_OF_DATE_KHR) {
    resize_requested = true;
    return core::Status::RETRYABLE_ERROR;
  } else if (vk_err && vk_err != VK_SUBOPTIMAL_KHR) {
    logger.log("ready_swapchain failed acquiring image");
    return core::Status::ERROR;
  }
  // success
  return core::Status::SUCCESS;
}

core::Status
mgmt::vulkan::Manager::resize_swapchain(swapchain::Swapchain& swapchain)
{
  vkDeviceWaitIdle(device_);
  _destroy_swapchain(swapchain);
  int w, h; // move
  SDL_GetWindowSize(window_mgr_->get_window(), &w, &h);
  vkb::SwapchainBuilder swapchainBuilder{ gpu_, device_, surface_ };
  swapchain.image_fmt = VK_FORMAT_B8G8R8A8_UNORM;
  vkb::Swapchain vkb_swapchain =
    swapchainBuilder
      //.use_default_format_selection()
      .set_desired_format(
        VkSurfaceFormatKHR{ .format = swapchain.image_fmt,
                            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
      // use vsync present mode
      .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
      .set_desired_extent(w, h)
      .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
      .build()
      .value();
  swapchain.extent = vkb_swapchain.extent;
  // store swapchain and its related images
  swapchain.swapchain = vkb_swapchain.swapchain;
  swapchain.imgs = vkb_swapchain.get_images().value();
  swapchain.img_views = vkb_swapchain.get_image_views().value();
  resize_requested = false;
  return core::Status::SUCCESS;
}
