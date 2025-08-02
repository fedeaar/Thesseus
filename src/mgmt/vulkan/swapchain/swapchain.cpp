#include "swapchain.h"

//
// constructor
//

core::code
mgmt::vulkan::Swapchain::create_frames()
{
  auto fence_info = info::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
  auto semaphore_info = mgmt::vulkan::info::semaphore_create_info();
  auto command_pool_info = mgmt::vulkan::info::command_pool_create_info(
    vk_mgr_->get_graphics_queue_family(),
    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  std::vector<descriptor::PoolSizeRatio> frame_sizes = {
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 },
  };
  for (u32 i = 0; i < FRAME_OVERLAP; ++i) {
    if (vk_mgr_->create_descriptors(
          frame_sizes, 1000, frames[i].frame_descriptors) !=
        core::code::SUCCESS) {
      return core::code::ERROR;
    }
    if (vk_mgr_->create_command_buffers(command_pool_info,
                                        frames[i].command_pool,
                                        frames[i].main_command_buffer) !=
        core::code::SUCCESS) {
      return core::code::ERROR;
    }

    if (vk_mgr_->create_fence(fence_info, frames[i].render_fence) !=
        core::code::SUCCESS) {
      return core::code::ERROR;
    }
    if (vk_mgr_->create_semaphore(semaphore_info,
                                  frames[i].swapchain_semaphore) !=
        core::code::SUCCESS) {
      return core::code::ERROR;
    }
    if (vk_mgr_->create_semaphore(semaphore_info, frames[i].render_semaphore) !=
        core::code::SUCCESS) {
      return core::code::ERROR;
    }
  }
  del_queue_.push([&]() mutable {
    vk_mgr_->device_wait_idle();
    for (u32 i = 0; i < FRAME_OVERLAP; ++i) {
      frames[i].del_queue.flush();
    }
  });
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Swapchain::init()
{
  if (initialized != core::status::NOT_INITIALIZED) {
    core::Logger::err("mgmt::vulkan::Swapchain::init",
                      "called before initialization");
    return core::code::NOT_INITIALIZED;
  }
  if (initialized == core::status::ERROR) {
    core::Logger::err("mgmt::vulkan::Swapchain::init", "in error state");
    return core::code::IN_ERROR_STATE;
  }
  // set surface fmt
  surface_fmt =
    VkSurfaceFormatKHR{ .format = VK_FORMAT_B8G8R8A8_UNORM,
                        .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
  // create basic swapchain
  if (vk_mgr_->create_swapchain(
        surface_fmt, extent, swapchain, imgs, imgs_views) !=
      core::code::SUCCESS) {
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // create draw img
  VkExtent3D extent_3d = { extent.width, extent.height, 1 };
  if (vk_mgr_->create_image(
        extent_3d,
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
          VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        false,
        draw_img) != core::code::SUCCESS) {
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // create depth img
  if (vk_mgr_->create_image(extent_3d,
                            VK_FORMAT_D32_SFLOAT,
                            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                            false,
                            depth_img) != core::code::SUCCESS) {
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // create frames
  if (create_frames() != core::code::SUCCESS) {
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  mgmt::vulkan::descriptor::LayoutBuilder layout_builder;
  layout_builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
  if (vk_mgr_->create_descriptor_set(layout_builder,
                                     VK_SHADER_STAGE_COMPUTE_BIT,
                                     draw_img_descriptor_layout,
                                     draw_img_descriptors) !=
      core::code::SUCCESS) {
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // update descriptor sets
  descriptor::Writer writer;
  writer.write_image(0,
                     draw_img.view,
                     VK_NULL_HANDLE,
                     VK_IMAGE_LAYOUT_GENERAL,
                     VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
  if (vk_mgr_->update_descriptor_set(writer, draw_img_descriptors) !=
      core::code::SUCCESS) {
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // success
  initialized = core::status::INITIALIZED;
  return core::code::SUCCESS;
}

mgmt::vulkan::Swapchain::Swapchain(vulkan::Manager* vk_mgr)
  : vk_mgr_{ vk_mgr } {};

//
// destructor
//

core::code
mgmt::vulkan::Swapchain::destroy()
{
  if (initialized == core::status::NOT_INITIALIZED) {
    core::Logger::err("mgmt::vulkan::Manager::destroy",
                      "called before initialization");
    return core::code::SUCCESS;
  }
  del_queue_.flush();
  vk_mgr_->destroy_swapchain(swapchain, imgs, imgs_views);
  initialized = core::status::NOT_INITIALIZED;
  return core::code::SUCCESS;
}

mgmt::vulkan::Swapchain::~Swapchain()
{
  if (initialized == core::status::INITIALIZED) {
    destroy();
  }
}

//
// image
//

core::code
mgmt::vulkan::Swapchain::draw_img_transition(VkImageLayout current,
                                             VkImageLayout next)
{
  return image::transition_image(
    get_current_cmd_buffer(), draw_img.image, current, next);
}

core::code
mgmt::vulkan::Swapchain::depth_img_transition(VkImageLayout current,
                                              VkImageLayout next)
{
  return image::transition_image(
    get_current_cmd_buffer(), depth_img.image, current, next);
}

core::code
mgmt::vulkan::Swapchain::current_img_transition(VkImageLayout current,
                                                VkImageLayout next)
{
  return image::transition_image(
    get_current_cmd_buffer(), get_current_image(), current, next);
}

core::code
mgmt::vulkan::Swapchain::copy_draw_to_current()
{
  return image::copy_image(get_current_cmd_buffer(),
                           draw_img.image,
                           get_current_image(),
                           draw_extent,
                           extent);
}

//
// commands
//

core::code
mgmt::vulkan::Swapchain::begin_commands()
{
  // we assume we are init
  draw_extent.width =
    core::min(extent.width, draw_img.extent_2d.width) * render_scale;
  draw_extent.height =
    core::min(extent.height, draw_img.extent_2d.height) * render_scale;
  auto& current_frame = get_current_frame();
  auto device = vk_mgr_->get_dev();
  // wait and prepare for next render
  if (vk_mgr_->await(
        1, &current_frame.render_fence, true, 1000000000 /* 1s */) !=
      core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Swapchain::begin_commands",
                      "failed awaiting render fence");
    return core::code::ERROR;
  }
  current_frame.del_queue.flush();
  current_frame.frame_descriptors.clear_pools(device);
  auto vk_err = vkAcquireNextImageKHR(device,
                                      swapchain,
                                      1000000000,
                                      current_frame.swapchain_semaphore,
                                      nullptr,
                                      &current_img_idx);
  if (vk_err == VK_ERROR_OUT_OF_DATE_KHR) {
    resize_requested = true;
    return core::code::RETRYABLE_ERROR;
  } else if (vk_err && vk_err != VK_SUBOPTIMAL_KHR) {
    core::Logger::log("mgmt::vulkan::Swapchain::begin_commands",
                      "failed acquiring image");
    return core::code::ERROR;
  }
  auto status = check(vkResetFences(device, 1, &current_frame.render_fence));
  if (status != core::code::SUCCESS) {
    core::Logger::log("mgmt::vulkan::Swapchain::begin_commands",
                      "failed resetting fence");
    return core::code::ERROR;
  }
  auto cmd = get_current_cmd_buffer();
  status = check(vkResetCommandBuffer(cmd, 0));
  if (status != core::code::SUCCESS) {
    core::Logger::log("mgmt::vulkan::Swapchain::begin_commands",
                      "failed resetting command buffer");
    return core::code::ERROR;
  }
  VkCommandBufferBeginInfo cmd_info = info::command_buffer_begin_info(
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  status = check(vkBeginCommandBuffer(cmd, &cmd_info));
  if (status != core::code::SUCCESS) {
    core::Logger::log("mgmt::vulkan::Swapchain::begin_commands",
                      "failed initializing command buffer");
    return core::code::ERROR;
  }
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Swapchain::end_commands()
{
  auto& cmd = get_current_cmd_buffer();
  auto& graphics_queue = vk_mgr_->get_graphics_queue();
  auto status = check(vkEndCommandBuffer(cmd));
  if (status != core::code::SUCCESS) {
    return core::code::ERROR; // todo@mgmt: log error msg
  }
  auto& current_frame = get_current_frame();
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
    vkQueueSubmit2(graphics_queue, 1, &submit, current_frame.render_fence));
  if (status != core::code::SUCCESS) {
    return core::code::ERROR; // todo@mgmt: log error msg
  }
  // display
  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.pNext = nullptr;
  present_info.pSwapchains = &swapchain;
  present_info.swapchainCount = 1;
  present_info.pWaitSemaphores = &current_frame.render_semaphore;
  present_info.waitSemaphoreCount = 1;
  present_info.pImageIndices = &current_img_idx;
  auto vk_err = vkQueuePresentKHR(graphics_queue, &present_info);
  frame++;
  if (vk_err == VK_ERROR_OUT_OF_DATE_KHR) {
    resize_requested = true;
    return core::code::RETRYABLE_ERROR;
  } else if (vk_err && vk_err != VK_SUBOPTIMAL_KHR) {
    core::Logger::log("mgmt::vulkan::Manager::swapchain_end_commands",
                      "failed acquiring image");
    return core::code::ERROR;
  }
  // success
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Swapchain::set_viewport_and_sissor()
{
  auto& cmd = get_current_cmd_buffer();
  VkViewport viewport = {};
  viewport.x = 0;
  viewport.y = 0;
  viewport.width = draw_extent.width;
  viewport.height = draw_extent.height;
  viewport.minDepth = 0.f;
  viewport.maxDepth = 1.f;
  vkCmdSetViewport(cmd, 0, 1, &viewport);
  VkRect2D scissor = {};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent.width = viewport.width;
  scissor.extent.height = viewport.height;
  vkCmdSetScissor(cmd, 0, 1, &scissor);
}

//
// resize
//

core::code
mgmt::vulkan::Swapchain::resize_extent()
{
  // we assume init
  if (!resize_requested) {
    return core::code::SUCCESS;
  }
  if (vk_mgr_->destroy_swapchain(swapchain, imgs, imgs_views) !=
      core::code::SUCCESS) {
    return core::code::ERROR;
  }
  if (vk_mgr_->create_swapchain(
        surface_fmt, extent, swapchain, imgs, imgs_views) !=
      core::code::SUCCESS) {
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  resize_requested = false;
  return core::code::SUCCESS;
}

//
// get
//

mgmt::vulkan::Swapchain::Frame&
mgmt::vulkan::Swapchain::get_current_frame()
{
  return frames[frame % FRAME_OVERLAP];
}

VkImage&
mgmt::vulkan::Swapchain::get_current_image()
{
  return imgs[current_img_idx];
}

VkImageView&
mgmt::vulkan::Swapchain::get_current_image_view()
{
  return imgs_views[current_img_idx];
}

VkCommandBuffer&
mgmt::vulkan::Swapchain::get_current_cmd_buffer()
{
  return get_current_frame().main_command_buffer;
}
