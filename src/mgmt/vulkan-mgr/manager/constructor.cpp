#define VMA_IMPLEMENTATION

#include "manager.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

ResourceManagement::Status
ResourceManagement::VulkanManager::Manager::init()
{
  if (initialized) {
    return ResourceManagement::Status::SUCCESS;
  }
  // auto window_mgr = *window_mgr_;
  // if (!window_mgr->initialized) {
  //   logger.error(fmt::format("init failed, window_mgr is not initialized"));
  //   return ResourceManagement::Status::ERROR;
  // }
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    logger.error(fmt::format("Failed to initialize: %s\n", SDL_GetError()));
    return Status::ERROR;
  }
  // Create window
  u32 flags = (u32)(SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED);
  window_ = SDL_CreateWindow("abc", 1024, 720, flags);
  if (window_ == NULL) {
    logger.error(fmt::format("Failed to create window: %s\n", SDL_GetError()));
    return Status::ERROR;
  }
  // load system info
  auto system_info_result = vkb::SystemInfo::get_system_info();
  if (!system_info_result.has_value()) {
    logger.error(fmt::format("init failed, get_system_info error: {}",
                             system_info_result.error().message()));
    return ResourceManagement::Status::ERROR;
  }
  auto system_info = system_info_result.value();
  // get required extensions
  std::vector<const char*> extensions;
  u32 extensions_count;
  auto extension_ptr = SDL_Vulkan_GetInstanceExtensions(&extensions_count);
  if (extension_ptr == NULL) {
    logger.error(
      fmt::format("get_required_extensions failed, error: {}", SDL_GetError()));
    return Status::ERROR;
  }
  if (!system_info.is_extension_available(VK_EXT_DEBUG_REPORT_EXTENSION_NAME)) {
    logger.error(
      fmt::format("init failed, required extension not available: {}",
                  VK_EXT_DEBUG_REPORT_EXTENSION_NAME));
    return ResourceManagement::Status::ERROR;
  }
  extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  for (int i = 0; i < extensions_count; ++i) {
    if (!system_info.is_extension_available(extension_ptr[i])) {
      logger.error(fmt::format(
        "init failed, required extension not available: {}", extension_ptr[i]));
      return ResourceManagement::Status::ERROR;
    }
    extensions.push_back(extension_ptr[i]);
  }
  // build instance
  vkb::InstanceBuilder builder;
  auto instance_result =
    builder.set_app_name("abc")
      .request_validation_layers(true) // todo@engine: debug only
      .use_default_debug_messenger()   // todo@engine: debug only
      .require_api_version(1, 3, 0)
      .enable_extensions(extensions)
      .build();
  if (!instance_result.has_value()) {
    logger.error(fmt::format("init failed, could not build instance: {}",
                             instance_result.error().message()));
    return ResourceManagement::Status::ERROR;
  }
  vkb::Instance vkb_instance = instance_result.value();
  instance_ = vkb_instance.instance;
  debug_messenger_ = vkb_instance.debug_messenger;
  // create surface
  if (!SDL_Vulkan_CreateSurface(window_, instance_, nullptr, &surface_)) {
    logger.error(fmt::format("build_surface failed to create surface, {}",
                             SDL_GetError()));
    return ResourceManagement::Status::ERROR;
  }
  // select physical device
  VkPhysicalDeviceVulkan12Features features_1d2 = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES
  };
  features_1d2.bufferDeviceAddress = true;
  features_1d2.descriptorIndexing = true;
  VkPhysicalDeviceVulkan13Features features_1d3 = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES
  };
  features_1d3.dynamicRendering = true;
  features_1d3.synchronization2 = true;
  vkb::PhysicalDeviceSelector selector{ vkb_instance };
  auto selector_result = selector.set_minimum_version(1, 3)
                           .set_required_features_13(features_1d3)
                           .set_required_features_12(features_1d2)
                           //.add_required_extensions(extensions)
                           .set_surface(surface_)
                           .select();
  if (!selector_result.has_value()) {
    logger.error(
      fmt::format("init failed, could not select physical device: {}",
                  selector_result.error().message()));
    return ResourceManagement::Status::ERROR;
  }
  auto vkb_phys_device = selector_result.value();
  // build device
  vkb::DeviceBuilder device_builder{ vkb_phys_device };
  auto device_builder_result = device_builder.build();
  if (!device_builder_result.has_value()) {
    logger.error(fmt::format("init failed, could not build device: {}",
                             device_builder_result.error().message()));
    return ResourceManagement::Status::ERROR;
  }
  auto vkb_device = device_builder_result.value();
  device_ = vkb_device.device;
  gpu_ = vkb_phys_device.physical_device;
  // get graphics queue
  auto vkb_graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics);
  if (!vkb_graphics_queue.has_value()) {
    logger.error(fmt::format("init failed, could not get graphics queue: {}",
                             vkb_graphics_queue.error().message()));
    return ResourceManagement::Status::ERROR;
  }
  graphics_queue_ = vkb_graphics_queue.value();
  auto vkb_queue_index = vkb_device.get_queue_index(vkb::QueueType::graphics);
  if (!vkb_queue_index.has_value()) {
    logger.error(
      fmt::format("init failed, could not get graphics queue index: {}",
                  vkb_queue_index.error().message()));
    return ResourceManagement::Status::ERROR;
  }
  graphics_queue_family_ = vkb_queue_index.value();
  // create allocator
  VmaAllocatorCreateInfo allocator_info = {};
  allocator_info.physicalDevice = gpu_;
  allocator_info.device = device_;
  allocator_info.instance = instance_;
  allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
  vmaCreateAllocator(&allocator_info, &allocator_);
  // create descriptors
  Descriptor::Allocator::PoolSizeRatio sizes[1] = {
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
  };
  descriptor_allocator_.init_pool(device_, 10, sizes);
  // setup destroyers
  del_queue_.push([=]() {
    logger.error("called manager destroy");
    descriptor_allocator_.destroy_pool(device_);
    vmaDestroyAllocator(allocator_);
    vkDestroyDevice(device_, nullptr);
    vkb::destroy_debug_utils_messenger(instance_, debug_messenger_);
    vkDestroyInstance(instance_, nullptr);
  });
  // success
  initialized = true;
  return ResourceManagement::Status::SUCCESS;
}

ResourceManagement::VulkanManager::Manager::Manager(WindowManager* window_mgr)
  : window_mgr_{ window_mgr } {};

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
