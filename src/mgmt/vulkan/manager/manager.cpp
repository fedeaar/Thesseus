
#include "manager.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

//
// constructor
//

core::code
mgmt::vulkan::Manager::init()
{
  if (initialized == core::status::INIT) {
    return core::code::SUCCESS;
  }
  if (!window_mgr_->initialized) {
    core::Logger::err("mgmt::vulkan::Manager::init",
                      "window_mgr is not initialized");
    return core::code::ERROR;
  }
  // load system info
  auto system_info_result = vkb::SystemInfo::get_system_info();
  if (!system_info_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::init",
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
    core::Logger::err("mgmt::vulkan::Manager::init",
                      "get_required_extensions failed, error: {}",
                      SDL_GetError());
    return core::code::ERROR;
  }
  if (!system_info.is_extension_available(VK_EXT_DEBUG_REPORT_EXTENSION_NAME)) {
    core::Logger::err("mgmt::vulkan::Manager::init",
                      "required extension not available: {}",
                      VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    return core::code::ERROR;
  }
  extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  for (int i = 0; i < extensions_count; ++i) {
    if (!system_info.is_extension_available(extension_ptr[i])) {
      core::Logger::err("mgmt::vulkan::Manager::init",
                        "required extension not available: {}",
                        extension_ptr[i]);
      return core::code::ERROR;
    }
    extensions.push_back(extension_ptr[i]);
  }
  // build instance
  vkb::InstanceBuilder builder;
  auto instance_result =
    builder.set_app_name(window_mgr_->window_name.c_str())
      .request_validation_layers(true) // todo@engine: debug only
      .use_default_debug_messenger()   // todo@engine: debug only
      .require_api_version(1, 3, 0)
      .enable_extensions(extensions)
      .build();
  if (!instance_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::init",
                      "could not build instance: {}",
                      instance_result.error().message());
    return core::code::ERROR;
  }
  vkb::Instance vkb_instance = instance_result.value();
  instance_ = vkb_instance.instance;
  debug_messenger_ = vkb_instance.debug_messenger;
  // create surface
  if (window_mgr_->build_surface(instance_, &surface_) != core::code::SUCCESS) {
    return core::code::ERROR;
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
                           .set_surface(surface_)
                           .select();
  if (!selector_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::init",
                      "could not select physical device: {}",
                      selector_result.error().message());
    return core::code::ERROR;
  }
  auto vkb_phys_device = selector_result.value();
  // build device
  vkb::DeviceBuilder device_builder{ vkb_phys_device };
  auto device_builder_result = device_builder.build();
  if (!device_builder_result.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::init",
                      "could not build device: {}",
                      device_builder_result.error().message());
    return core::code::ERROR;
  }
  auto vkb_device = device_builder_result.value();
  device_ = vkb_device.device;
  gpu_ = vkb_phys_device.physical_device;
  // get graphics queue
  auto vkb_graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics);
  if (!vkb_graphics_queue.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::init",
                      "could not get graphics queue: {}",
                      vkb_graphics_queue.error().message());
    return core::code::ERROR;
  }
  graphics_queue_ = vkb_graphics_queue.value();
  auto vkb_queue_index = vkb_device.get_queue_index(vkb::QueueType::graphics);
  if (!vkb_queue_index.has_value()) {
    core::Logger::err("mgmt::vulkan::Manager::init",
                      "could not get graphics queue index: {}",
                      vkb_queue_index.error().message());
    return core::code::ERROR;
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
  descriptor::PoolSizeRatio sizes[1] = { { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                           1 } };
  descriptor_allocator_.init_pool(device_, 10, sizes);
  // create imm submit
  VkFenceCreateInfo fence_info =
    info::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
  auto status =
    check(vkCreateFence(device_, &fence_info, nullptr, &imm_submit_.fence));
  if (status != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::init",
                      "could not create imm fence: {}",
                      vkb_queue_index.error().message());
    return core::code::ERROR;
  }
  VkCommandPoolCreateInfo pool_info = info::command_pool_create_info(
    graphics_queue_family_, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  status = check(vkCreateCommandPool(
    device_, &pool_info, nullptr, &imm_submit_.command_pool));
  if (status != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::init",
                      "could not create imm command pool: {}",
                      vkb_queue_index.error().message());
    return core::code::ERROR;
  }
  VkCommandBufferAllocateInfo cmd_info =
    info::command_buffer_allocate_info(imm_submit_.command_pool, 1);
  status = check(
    vkAllocateCommandBuffers(device_, &cmd_info, &imm_submit_.command_buffer));
  if (status != core::code::SUCCESS) {
    core::Logger::err("mgmt::vulkan::Manager::init",
                      "could not alloc imm buffer: {}",
                      vkb_queue_index.error().message());
    return core::code::ERROR;
  }
  // setup destroyers
  del_queue_.push([=]() {
    vkDeviceWaitIdle(device_);
    vkDestroyCommandPool(device_, imm_submit_.command_pool, nullptr);
    vkDestroyFence(device_, imm_submit_.fence, nullptr);
    descriptor_allocator_.destroy_pool(device_);
    vmaDestroyAllocator(allocator_);
    vkDestroySurfaceKHR(instance_, surface_, nullptr);
    vkDestroyDevice(device_, nullptr);
    vkb::destroy_debug_utils_messenger(instance_, debug_messenger_);
    vkDestroyInstance(instance_, nullptr);
  });
  // success
  initialized = core::status::INIT;
  return core::code::SUCCESS;
}

mgmt::vulkan::Manager::Manager(WindowManager* window_mgr)
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
  destroy();
};

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
