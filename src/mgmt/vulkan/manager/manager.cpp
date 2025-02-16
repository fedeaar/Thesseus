
#include "manager.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

//
// constructor
//

core::code
mgmt::vulkan::Manager::init_instance(vkb::Instance& vkb_instance)
{
  // load system info
  auto system_info_result = vkb::SystemInfo::get_system_info();
  if (!system_info_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::init_instance",
                      "get_system_info error: {}",
                      system_info_result.error().message());
    return core::code::ERROR;
  }
  auto system_info = system_info_result.value();
  // get required extensions
  std::vector<const char*> extensions;
  u32 extensions_count;
  auto extension_ptr = SDL_Vulkan_GetInstanceExtensions(&extensions_count);
  if (extension_ptr == NULL) {
    core::Logger::err("mgmt::vulkan::Manager::init_instance",
                      "get_required_extensions failed, error: {}",
                      SDL_GetError());
    return core::code::ERROR;
  }
  if (!system_info.is_extension_available(VK_EXT_DEBUG_REPORT_EXTENSION_NAME)) {
    core::Logger::err("mgmt::vulkan::Manager::init_instance",
                      "required extension not available: {}",
                      VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    return core::code::ERROR;
  }
  extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  for (int i = 0; i < extensions_count; ++i) {
    if (!system_info.is_extension_available(extension_ptr[i])) {
      core::Logger::err("mgmt::vulkan::Manager::init_instance",
                        "required extension not available: {}",
                        extension_ptr[i]);
      return core::code::ERROR;
    }
    extensions.push_back(extension_ptr[i]);
  }
  // build instance
  vkb::InstanceBuilder builder;
  auto instance_result =
    builder.set_app_name(window_mgr_->state.window_name.c_str())
      .request_validation_layers(true) // TODO: debug only
      .use_default_debug_messenger()   // TODO: debug only
      .require_api_version(1, 3, 0)
      .enable_extensions(extensions)
      .build();
  if (!instance_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::init_instance",
                      "could not build instance: {}",
                      instance_result.error().message());
    return core::code::ERROR;
  }
  vkb_instance = instance_result.value();
  instance_ = vkb_instance.instance;
  debug_messenger_ = vkb_instance.debug_messenger;
  del_queue_.push([&]() {
    vkb::destroy_debug_utils_messenger(instance_, debug_messenger_);
    vkDestroyInstance(instance_, nullptr);
  });
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Manager::init_surface()
{
  auto result = window_mgr_->build_surface(instance_, &surface_);
  if (result == core::code::SUCCESS) {
    del_queue_.push(
      [&]() { vkDestroySurfaceKHR(instance_, surface_, nullptr); });
  }
  return result;
}

core::code
mgmt::vulkan::Manager::init_devices(vkb::Instance const& vkb_instance,
                                    vkb::Device& vkb_dev)
{ // select physical device
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
                           .set_surface(surface_)
                           .select();
  if (!selector_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::init_devices",
                      "could not select physical device: {}",
                      selector_result.error().message());
    return core::code::ERROR;
  }
  auto vkb_phys_device = selector_result.value();
  gpu_ = vkb_phys_device.physical_device;
  // build device
  vkb::DeviceBuilder device_builder{ vkb_phys_device };
  auto device_builder_result = device_builder.build();
  if (!device_builder_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::init_devices",
                      "could not build device: {}",
                      device_builder_result.error().message());
    return core::code::ERROR;
  }
  vkb_dev = device_builder_result.value();
  device_ = vkb_dev.device;
  del_queue_.push([&]() { vkDestroyDevice(device_, nullptr); });
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Manager::init_graphics_queue(vkb::Device const& vkb_dev)
{
  // get graphics queue
  auto vkb_graphics_queue = vkb_dev.get_queue(vkb::QueueType::graphics);
  if (!vkb_graphics_queue.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::init_graphics_queue",
                      "could not get graphics queue: {}",
                      vkb_graphics_queue.error().message());
    return core::code::ERROR;
  }
  graphics_queue_ = vkb_graphics_queue.value();
  auto vkb_queue_index = vkb_dev.get_queue_index(vkb::QueueType::graphics);
  if (!vkb_queue_index.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::init_graphics_queue",
                      "could not get graphics queue index: {}",
                      vkb_queue_index.error().message());
    return core::code::ERROR;
  }
  graphics_queue_family_ = vkb_queue_index.value();
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Manager::init_allocator()
{
  VmaAllocatorCreateInfo allocator_info = {};
  allocator_info.instance = instance_;
  allocator_info.physicalDevice = gpu_;
  allocator_info.device = device_;
  allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
  del_queue_.push([&]() { vmaDestroyAllocator(allocator_); });
  return check(vmaCreateAllocator(&allocator_info, &allocator_));
}

core::code
mgmt::vulkan::Manager::init_descriptors()
{
  descriptor::PoolSizeRatio sizes[1] = { { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                           1 } };
  auto result = descriptor_allocator_.init_pool(device_, 10, sizes);
  if (result == core::code::SUCCESS) {
    del_queue_.push([&]() { descriptor_allocator_.destroy_pool(device_); });
  }
  return result;
}

core::code
mgmt::vulkan::Manager::init_imm_submit()
{
  VkFenceCreateInfo fence_info =
    info::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
  auto status =
    check(vkCreateFence(device_, &fence_info, nullptr, &imm_submit_.fence));
  if (status != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::init_imm_submit",
                      "could not create imm fence");
    return core::code::ERROR;
  }
  VkCommandPoolCreateInfo pool_info = info::command_pool_create_info(
    graphics_queue_family_, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  status = check(vkCreateCommandPool(
    device_, &pool_info, nullptr, &imm_submit_.command_pool));
  if (status != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::init_imm_submit",
                      "could not create imm command pool");
    return core::code::ERROR;
  }
  VkCommandBufferAllocateInfo cmd_info =
    info::command_buffer_allocate_info(imm_submit_.command_pool, 1);
  status = check(
    vkAllocateCommandBuffers(device_, &cmd_info, &imm_submit_.command_buffer));
  if (status != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::init_imm_submit",
                      "could not alloc imm buffer");
    return core::code::ERROR;
  }
  del_queue_.push([&]() {
    vkDestroyCommandPool(device_, imm_submit_.command_pool, nullptr);
    vkDestroyFence(device_, imm_submit_.fence, nullptr);
  });
}

core::code
mgmt::vulkan::Manager::init()
{
  if (initialized == core::status::INIT) {
    return core::code::SUCCESS;
  }
  if (initialized == core::status::ERROR) {
    core::Logger::wrn("mgmt::vulkan::Manager::init", "in error state");
    return core::code::IN_ERROR_STATE;
  }
  if (window_mgr_->state.status != core::status::INIT) {
    core::Logger::err("mgmt::vulkan::Manager::init",
                      "window_mgr is not initialized");
    return core::code::ERROR;
  }
  vkb::Instance vkb_instance;
  if (init_instance(vkb_instance) != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  if (init_surface() != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  vkb::Device vkb_device;
  if (init_devices(vkb_instance, vkb_device) != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  if (init_graphics_queue(vkb_device) != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  if (init_allocator() != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  if (init_descriptors() != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  if (init_imm_submit() != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  // success
  initialized = core::status::INIT;
  return core::code::SUCCESS;
}

mgmt::vulkan::Manager::Manager(window::Manager* window_mgr)
  : window_mgr_{ window_mgr } {};

//
// destructor
//

core::code
mgmt::vulkan::Manager::destroy()
{
  if (initialized == core::status::NOT_INIT) {
    core::Logger::err("mgmt::vulkan::Manager::destroy",
                      "called before initialization");
    return core::code::SUCCESS;
  }
  device_wait_idle();
  del_queue_.flush();
  initialized = core::status::NOT_INIT;
  return core::code::SUCCESS;
};

mgmt::vulkan::Manager::~Manager()
{
  if (initialized == core::status::INIT) {
    destroy();
  }
};

//
// swapchain
//

core::code
mgmt::vulkan::Manager::create_swapchain(VkSurfaceFormatKHR surface_fmt,
                                        VkExtent2D& extent,
                                        VkSwapchainKHR& swapchain,
                                        std::vector<VkImage>& imgs,
                                        std::vector<VkImageView>& imgs_views)
{
  if (window_mgr_->state.status != core::status::INIT) {
    core::Logger::err("mgmt::vulkan::Manager::create_swapchain",
                      "window_mgr is not initialized");
    return core::code::ERROR;
  }
  window_mgr_->get_extent(extent);
  vkb::SwapchainBuilder swapchain_builder{ gpu_, device_, surface_ };
  auto vkb_swapchain_result =
    swapchain_builder.set_desired_format(surface_fmt)
      .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
      .set_desired_extent(extent.width, extent.height)
      .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
      .build();
  if (!vkb_swapchain_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::create_swapchain",
                      "swapchain builder error: {}",
                      vkb_swapchain_result.error().message());
    return core::code::ERROR;
  }
  auto vkb_swapchain = vkb_swapchain_result.value();
  extent = vkb_swapchain.extent;
  swapchain = vkb_swapchain.swapchain;
  // create image
  auto vkb_swapchain_imgs_result = vkb_swapchain.get_images();
  if (!vkb_swapchain_imgs_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Swapchain::init",
                      "get_images error: {}",
                      vkb_swapchain_imgs_result.error().message());
    return core::code::ERROR;
  }
  imgs = vkb_swapchain_imgs_result.value();
  // create image views
  auto vkb_swapchain_img_views_result = vkb_swapchain.get_image_views();
  if (!vkb_swapchain_img_views_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Swapchain::init",
                      "get_image_views error: {}",
                      vkb_swapchain_img_views_result.error().message());
    return core::code::ERROR;
  }
  imgs_views = vkb_swapchain_img_views_result.value();
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Manager::destroy_swapchain(VkSwapchainKHR& swapchain,
                                         std::vector<VkImage>& imgs,
                                         std::vector<VkImageView>& imgs_views)
{
  device_wait_idle();
  vkDestroySwapchainKHR(device_, swapchain, nullptr);
  for (auto& img_view : imgs_views) {
    vkDestroyImageView(device_, img_view, nullptr);
  }
  for (auto& img : imgs) {
    vkDestroyImage(device_, img, nullptr);
  }
  return core::code::SUCCESS;
}

//
// image
//

core::code
mgmt::vulkan::Manager::create_image(VkExtent3D extent,
                                    VkFormat format,
                                    VkImageUsageFlags usage,
                                    bool mipmapped,
                                    image::AllocatedImage& image)
{
  // create image
  image.format = format;
  image.extent_2d = { extent.width, extent.height };
  image.extent_3d = extent;
  VkImageCreateInfo img_info = info::image_create_info(format, usage, extent);
  if (mipmapped) {
    img_info.mipLevels = static_cast<u32>(std::floor(
                           std::log2(std::max(extent.width, extent.height)))) +
                         1;
  }
  VmaAllocationCreateInfo alloc_info = {};
  alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  alloc_info.requiredFlags =
    VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  auto status = check(vmaCreateImage(allocator_,
                                     &img_info,
                                     &alloc_info,
                                     &image.image,
                                     &image.allocation,
                                     nullptr));
  if (status != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::create_image",
                      "failed vmaCreateImage");
    return core::code::ERROR;
  }
  // create view
  auto aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;
  if (format == VK_FORMAT_D32_SFLOAT) {
    aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
  }
  VkImageViewCreateInfo view_info =
    info::imageview_create_info(format, image.image, aspect_flags);
  view_info.subresourceRange.levelCount = img_info.mipLevels;
  status = check(vkCreateImageView(device_, &view_info, nullptr, &image.view));
  if (status != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  del_queue_.push([=]() mutable {
    vkDestroyImageView(device_, image.view, nullptr);
    vmaDestroyImage(allocator_, image.image, image.allocation);
  });
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Manager::create_image(void* data,
                                    VkExtent3D extent,
                                    VkFormat format,
                                    VkImageUsageFlags usage,
                                    bool mipmapped,
                                    image::AllocatedImage& image)
{
  u32 data_size = extent.depth * extent.width * extent.height * 4; // FIXME
  auto buff_result = create_buffer(
    data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
  if (!buff_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::create_image",
                      "create_buffer failed");
    return core::code::ERROR;
  }
  buffer::AllocatedBuffer upload_buff = buff_result.value();
  memcpy(upload_buff.info.pMappedData, data, data_size);
  if (create_image(extent,
                   format,
                   usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                     VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                   mipmapped,
                   image) != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::create_image",
                      "create_image overload failed");
    return core::code::ERROR;
  }
  auto status = imm_submit([&](VkCommandBuffer cmd) {
    image::transition_image(cmd,
                            image.image,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    VkBufferImageCopy cpy_region = {};
    cpy_region.bufferOffset = 0;
    cpy_region.bufferRowLength = 0;
    cpy_region.bufferImageHeight = 0;
    cpy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    cpy_region.imageSubresource.mipLevel = 0;
    cpy_region.imageSubresource.baseArrayLayer = 0;
    cpy_region.imageSubresource.layerCount = 1;
    cpy_region.imageExtent = extent;
    vkCmdCopyBufferToImage(cmd,
                           upload_buff.buffer,
                           image.image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &cpy_region);
    image::transition_image(cmd,
                            image.image,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  });
  if (status != core::code::SUCCESS) {
    // TODO: destroy buffer nevertheless
    core::Logger::err("mgmt::vulkan::Manager::create_image",
                      "images setup overload failed");
    return core::code::ERROR;
  }
  status = destroy_buffer(upload_buff);
  if (status != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::create_image",
                      "destroy_buffer failed");
    return core::code::ERROR;
  }
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Manager::destroy_image(const image::AllocatedImage& img)
{
  vkDestroyImageView(device_, img.view, nullptr);
  vmaDestroyImage(allocator_, img.image, img.allocation);
  return core::code::SUCCESS;
}

//
// sync
//

core::code
mgmt::vulkan::Manager::create_fence(VkFenceCreateInfo& info, VkFence& fence)
{
  auto status = check(vkCreateFence(device_, &info, nullptr, &fence));
  if (status != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::create_fence",
                      "vkCreateFence error");
    return status;
  }
  del_queue_.push([=]() mutable { vkDestroyFence(device_, fence, nullptr); });
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Manager::create_semaphore(VkSemaphoreCreateInfo& info,
                                        VkSemaphore& semaphore)
{
  auto status = check(vkCreateSemaphore(device_, &info, nullptr, &semaphore));
  if (status != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::create_semaphore",
                      "vkCreateSemaphore error");
    return status;
  }
  del_queue_.push(
    [=]() mutable { vkDestroySemaphore(device_, semaphore, nullptr); });
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Manager::await(u32 count,
                             VkFence* fences,
                             bool wait_all,
                             u32 timeout)
{
  return check(vkWaitForFences(device_, 1, fences, wait_all, timeout));
}

//
// command pool
//

core::code
mgmt::vulkan::Manager::create_command_buffers(VkCommandPoolCreateInfo& info,
                                              VkCommandPool& pool,
                                              VkCommandBuffer& buffer)
{
  if (check(vkCreateCommandPool(device_, &info, nullptr, &pool)) !=
      core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::create_command_buffers",
                      "vkCreateCommandPool error");
    return core::code::ERROR;
  }
  auto command_buffer_alloc_info =
    mgmt::vulkan::info::command_buffer_allocate_info(pool, 1);
  if (check(vkAllocateCommandBuffers(
        device_, &command_buffer_alloc_info, &buffer)) != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::create_command_buffers",
                      "vkAllocateCommandBuffers error");
    return core::code::ERROR;
  }
  del_queue_.push(
    [=]() mutable { vkDestroyCommandPool(device_, pool, nullptr); });
  return core::code::SUCCESS;
}

//
// descriptors
//

core::code
mgmt::vulkan::Manager::create_descriptors(
  std::span<descriptor::PoolSizeRatio> const& init_ratios,
  u32 set_count,
  descriptor::DynamicAllocator& descriptor)
{
  descriptor = descriptor::DynamicAllocator{};
  descriptor.init(device_, set_count, init_ratios);
  del_queue_.push([=]() mutable { descriptor.destroy_pools(device_); });
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Manager::create_descriptor_set(descriptor::LayoutBuilder& builder,
                                             VkShaderStageFlags flags,
                                             VkDescriptorSetLayout& layout,
                                             VkDescriptorSet& descriptor_set)
{
  // build draw_img layout
  auto layout_result = builder.build(device_, flags);
  if (!layout_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::create_descriptor_set",
                      "layout_builder error");
    return layout_result.error();
  }
  layout = layout_result.value();
  // allocate a descriptor set
  auto descriptor_result = descriptor_allocator_.allocate(device_, layout);
  if (!descriptor_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::create_descriptor_set",
                      "descriptor_allocator error");
    return descriptor_result.error();
  }
  descriptor_set = descriptor_result.value();
  del_queue_.push(
    [=]() mutable { vkDestroyDescriptorSetLayout(device_, layout, nullptr); });
  return core::code::SUCCESS;
}

core::code
mgmt::vulkan::Manager::update_descriptor_set(descriptor::Writer& writer,
                                             VkDescriptorSet& descriptor_set)
{
  writer.update_set(device_, descriptor_set);
}

//
// dev
//

core::code
mgmt::vulkan::Manager::device_wait_idle()
{
  if (initialized != core::status::INIT) {
    return core::code::NOT_INIT;
  }
  return check(vkDeviceWaitIdle(device_));
}

//
// get
//

VkInstance const&
mgmt::vulkan::Manager::get_instance()
{
  return instance_;
}

VkPhysicalDevice const&
mgmt::vulkan::Manager::get_physical_dev()
{
  return gpu_;
}

VkDevice const&
mgmt::vulkan::Manager::get_dev()
{
  return device_;
}

VkQueue const&
mgmt::vulkan::Manager::get_graphics_queue()
{
  return graphics_queue_;
}

VmaAllocator const&
mgmt::vulkan::Manager::get_allocator()
{
  return allocator_;
}

u32
mgmt::vulkan::Manager::get_graphics_queue_family()
{
  return graphics_queue_family_;
}
