#define VMA_IMPLEMENTATION
#include "engine.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

VulkanRenderEngine::VulkanRenderEngine(RenderParams& params)
    : params_{params},
      aspect_ratio_{(f32)params_.screen_width / (f32)params_.screen_height},
      extent_{params_.screen_width, params_.screen_height} {}

FrameData& VulkanRenderEngine::get_current_frame() {
  return frames_[frame_ % FRAME_OVERLAP];
}

i32 VulkanRenderEngine::init_vulkan() {
  vkb::InstanceBuilder builder;
  auto inst_ret = builder.set_app_name(params_.name.c_str())
                      .request_validation_layers(true)
                      .use_default_debug_messenger()
                      .require_api_version(1, 3, 0)
                      .build();
  vkb::Instance vkb_inst = inst_ret.value();
  instance_ = vkb_inst.instance;
  debug_messenger_ = vkb_inst.debug_messenger;

  SDL_Vulkan_CreateSurface(window_, instance_, nullptr, &surface_);

  VkPhysicalDeviceVulkan13Features features{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
  features.dynamicRendering = true;
  features.synchronization2 = true;
  VkPhysicalDeviceVulkan12Features features12{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
  features12.bufferDeviceAddress = true;
  features12.descriptorIndexing = true;
  vkb::PhysicalDeviceSelector selector{vkb_inst};
  vkb::PhysicalDevice phys_dev = selector.set_minimum_version(1, 3)
                                     .set_required_features_13(features)
                                     .set_required_features_12(features12)
                                     .set_surface(surface_)
                                     .select()
                                     .value();
  vkb::DeviceBuilder device_builder{phys_dev};
  vkb::Device vkb_dev = device_builder.build().value();
  device_ = vkb_dev.device;
  gpu_ = phys_dev.physical_device;

  graphics_queue_ = vkb_dev.get_queue(vkb::QueueType::graphics).value();
  graphics_queue_family_ =
      vkb_dev.get_queue_index(vkb::QueueType::graphics).value();

  VmaAllocatorCreateInfo allocator_info = {};
  allocator_info.physicalDevice = gpu_;
  allocator_info.device = device_;
  allocator_info.instance = instance_;
  allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
  vmaCreateAllocator(&allocator_info, &allocator_);
  main_del_queue_.push_function([&]() { vmaDestroyAllocator(allocator_); });

  return 1;
}

i32 VulkanRenderEngine::create_swapchain(u32 width, u32 height) {
  vkb::SwapchainBuilder swapchain_builder{gpu_, device_, surface_};
  swapchain_image_fmt_ = VK_FORMAT_B8G8R8A8_UNORM;
  vkb::Swapchain vkb_swapchain =
      swapchain_builder
          //.use_default_format_selection()
          .set_desired_format(VkSurfaceFormatKHR{
              .format = swapchain_image_fmt_,
              .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
          // use vsync present mode
          .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
          .set_desired_extent(width, height)
          .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
          .build()
          .value();
  swapchain_extent_ = vkb_swapchain.extent;
  swapchain_ = vkb_swapchain.swapchain;
  swapchain_img_ = vkb_swapchain.get_images().value();
  swapchain_img_views_ = vkb_swapchain.get_image_views().value();
}

i32 VulkanRenderEngine::destroy_swapchain() {
  vkDestroySwapchainKHR(device_, swapchain_, nullptr);
  for (int i = 0; i < swapchain_img_views_.size(); i++) {
    vkDestroyImageView(device_, swapchain_img_views_[i], nullptr);
  }
  return 1;
}

i32 VulkanRenderEngine::init_swapchain() {
  create_swapchain(extent_.width, extent_.height);
  return 1;
}

i32 VulkanRenderEngine::init_commands() {
  VkCommandPoolCreateInfo command_pool_info = vkinit::command_pool_create_info(
      graphics_queue_family_, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  for (int i = 0; i < FRAME_OVERLAP; i++) {
    VK_CHECK(vkCreateCommandPool(device_, &command_pool_info, nullptr,
                                 &frames_[i].command_pool));
    VkCommandBufferAllocateInfo cmdAllocInfo =
        vkinit::command_buffer_allocate_info(frames_[i].command_pool, 1);
    VK_CHECK(vkAllocateCommandBuffers(device_, &cmdAllocInfo,
                                      &frames_[i].main_command_buffer));
  }
}

i32 VulkanRenderEngine::init_sync_structures() {
  VkFenceCreateInfo fence_info =
      vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
  VkSemaphoreCreateInfo semaphore_info = vkinit::semaphore_create_info(0);
  for (int i = 0; i < FRAME_OVERLAP; i++) {
    VK_CHECK(
        vkCreateFence(device_, &fence_info, nullptr, &frames_[i].render_fence));
    VK_CHECK(vkCreateSemaphore(device_, &semaphore_info, nullptr,
                               &frames_[i].swapchain_semaphore));
    VK_CHECK(vkCreateSemaphore(device_, &semaphore_info, nullptr,
                               &frames_[i].render_semaphore));
  }
}

i32 VulkanRenderEngine::init() {
  if (initialized_) {
    return 0;
  }
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Failed to initialize: %s\n", SDL_GetError());
    return 0;
  }
  // Create window
  u32 flags = (u32)(SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED);
  window_ = SDL_CreateWindow(params_.name.c_str(), extent_.width,
                             extent_.height, flags);
  if (window_ == NULL) {
    printf("Failed to create window: %s\n", SDL_GetError());
    return 0;
  }
  if (!init_vulkan()) {
    printf("Failed to initialize vulkan");
    return 0;
  }
  if (!init_swapchain()) {
    printf("Failed to initialize swapchain");
    return 0;
  }
  if (!init_commands()) {
    printf("Failed to initialize commands");
    return 0;
  }
  if (!init_sync_structures()) {
    printf("Failed to initialize sync structures");
    return 0;
  }
  initialized_ = true;
  return 1;
}

i32 VulkanRenderEngine::destroy() {
  if (!initialized_) {
    return 0;
  }
  vkDeviceWaitIdle(device_);
  for (int i = 0; i < FRAME_OVERLAP; i++) {
    vkDestroyCommandPool(device_, frames_[i].command_pool, nullptr);
    vkDestroyFence(device_, frames_[i].render_fence, nullptr);
    vkDestroySemaphore(device_, frames_[i].render_semaphore, nullptr);
    vkDestroySemaphore(device_, frames_[i].swapchain_semaphore, nullptr);
    frames_[i].del_queue.flush();
  }
  main_del_queue_.flush();
  destroy_swapchain();
  vkDestroySurfaceKHR(instance_, surface_, nullptr);
  vkDestroyDevice(device_, nullptr);
  vkb::destroy_debug_utils_messenger(instance_, debug_messenger_);
  vkDestroyInstance(instance_, nullptr);
  SDL_DestroyWindow(window_);
  window_ = nullptr;
  return 1;
};

void VulkanRenderEngine::render(const Camera& camera /*, Scene& scene*/) {
  // wait and prepare for next render
  VK_CHECK(vkWaitForFences(device_, 1, &get_current_frame().render_fence, true,
                           1000000000));  // 1 sec
  get_current_frame().del_queue.flush();
  VK_CHECK(vkResetFences(device_, 1, &get_current_frame().render_fence));
  u32 img_idx;
  VK_CHECK(vkAcquireNextImageKHR(device_, swapchain_, 1000000000,
                                 get_current_frame().swapchain_semaphore,
                                 nullptr, &img_idx));
  VkCommandBuffer cmd = get_current_frame().main_command_buffer;
  VK_CHECK(vkResetCommandBuffer(cmd, 0));
  VkCommandBufferBeginInfo cmd_info = vkinit::command_buffer_begin_info(
      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_info));
  // clear screen
  // make the swapchain image into writeable mode before rendering
  vkutil::transition_image(cmd, swapchain_img_[img_idx],
                           VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
  VkClearColorValue clear_val;
  float flash = std::abs(std::sin(frame_ / 120.f));
  clear_val = {{0.0f, 0.0f, flash, 1.0f}};
  VkImageSubresourceRange clear_range =
      vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
  vkCmdClearColorImage(cmd, swapchain_img_[img_idx], VK_IMAGE_LAYOUT_GENERAL,
                       &clear_val, 1, &clear_range);
  vkutil::transition_image(cmd, swapchain_img_[img_idx],
                           VK_IMAGE_LAYOUT_GENERAL,
                           VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  VK_CHECK(vkEndCommandBuffer(cmd));
  // prepare the submission to the queue
  VkCommandBufferSubmitInfo cmd_submit_info =
      vkinit::command_buffer_submit_info(cmd);
  VkSemaphoreSubmitInfo wait_info = vkinit::semaphore_submit_info(
      VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
      get_current_frame().swapchain_semaphore);
  VkSemaphoreSubmitInfo signal_info =
      vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                                    get_current_frame().render_semaphore);
  VkSubmitInfo2 submit =
      vkinit::submit_info(&cmd_submit_info, &signal_info, &wait_info);
  // submit
  VK_CHECK(vkQueueSubmit2(graphics_queue_, 1, &submit,
                          get_current_frame().render_fence));
  // display
  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.pNext = nullptr;
  present_info.pSwapchains = &swapchain_;
  present_info.swapchainCount = 1;
  present_info.pWaitSemaphores = &get_current_frame().render_semaphore;
  present_info.waitSemaphoreCount = 1;
  present_info.pImageIndices = &img_idx;
  VK_CHECK(vkQueuePresentKHR(graphics_queue_, &present_info));

  frame_++;
}

f32 VulkanRenderEngine::get_aspect_ratio() { return aspect_ratio_; }
