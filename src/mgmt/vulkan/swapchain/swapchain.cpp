#include "swapchain.h"

//
// constructor
//

core::code
mgmt::vulkan::Swapchain::create_frames(vulkan::Manager& vk_mgr)
{
  auto fence_info = info::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
  auto semaphore_info = mgmt::vulkan::info::semaphore_create_info();
  auto command_pool_info = mgmt::vulkan::info::command_pool_create_info(
    vk_mgr.get_graphics_queue_family(),
    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  std::vector<descriptor::PoolSizeRatio> frame_sizes = {
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 },
  };
  for (u32 i = 0; i < FRAME_OVERLAP; ++i) {
    if (vk_mgr.create_descriptors(
          frame_sizes, 1000, frames[i].frame_descriptors) !=
        core::code::SUCCESS) {
      return core::code::ERROR;
    }
    if (vk_mgr.create_command_buffers(command_pool_info,
                                      frames[i].command_pool,
                                      frames[i].main_command_buffer) !=
        core::code::SUCCESS) {
      return core::code::ERROR;
    }

    if (vk_mgr.create_fence(fence_info, frames[i].render_fence) !=
        core::code::SUCCESS) {
      return core::code::ERROR;
    }
    if (vk_mgr.create_semaphore(semaphore_info,
                                frames[i].swapchain_semaphore) !=
        core::code::SUCCESS) {
      return core::code::ERROR;
    }
    if (vk_mgr.create_semaphore(semaphore_info, frames[i].render_semaphore) !=
        core::code::SUCCESS) {
      return core::code::ERROR;
    }
  }
  del_queue_.push([&]() mutable {
    vk_mgr.device_wait_idle();
    for (u32 i = 0; i < FRAME_OVERLAP; ++i) {
      frames[i].del_queue.flush();
    }
  });
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Swapchain::init(vulkan::Manager& vk_mgr)
{
  if (initialized == core::status::NOT_INIT) {
    core::Logger::err("mgmt::vulkan::Swapchain::init",
                      "called before initialization");
    return core::code::NOT_INIT;
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
  if (vk_mgr.create_swapchain(
        surface_fmt, extent, swapchain, imgs, imgs_views) !=
      core::code::SUCCESS) {
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // create draw img
  VkExtent3D extent_3d = { extent.width, extent.height, 1 };
  if (vk_mgr.create_image(
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
  if (vk_mgr.create_image(extent_3d,
                          VK_FORMAT_D32_SFLOAT,
                          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                          false,
                          draw_img) != core::code::SUCCESS) {
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // create frames
  if (create_frames(vk_mgr) != core::code::SUCCESS) {
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  mgmt::vulkan::descriptor::LayoutBuilder layout_builder;
  layout_builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
  if (vk_mgr.create_descriptor_set(layout_builder,
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
  if (vk_mgr.update_descriptor_set(writer, draw_img_descriptors) !=
      core::code::SUCCESS) {
    initialized = core::status::ERROR;
    return core::code::ERROR;
  }
  // success
  initialized = core::status::INIT;
  return core::code::SUCCESS;
}

mgmt::vulkan::Swapchain(){};

//
// destructor
//

core::code
mgmt::vulkan::Swapchain::destroy()
{
  if (initialized == core::status::NOT_INIT) {
    core::Logger::err("mgmt::vulkan::Manager::destroy",
                      "called before initialization");
    return core::code::SUCCESS;
  }
  del_queue_.flush();
  initialized = core::status::NOT_INIT;
  return core::code::SUCCESS;
}

mgmt::vulkan::Swapchain::~Swapchain()
{
  if (initialized == core::status::INIT) {
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
