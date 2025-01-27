#include "manager.h"

using namespace RenderEngine;
using namespace RenderEngine::VulkanManager;

Status
Manager::init()
{
  if (initialized) {
    return Status::SUCCESS;
  }
  auto engine = *engine_;
  // get dependencies
  auto window_mgr_result = engine.get_window_mgr();
  if (!window_mgr_result.has_value()) {
    logger.error(
      "init failed, init called before WindowManager initialization.");
    return Status::ERROR;
  }
  auto window_mgr = window_mgr_result.value();
  // load system info
  auto system_info_result = vkb::SystemInfo::get_system_info();
  if (!system_info_result.has_value()) {
    logger.error(fmt::format("init failed, get_system_info error: {}",
                             system_info_result.full_error()));
    return Status::ERROR;
  }
  auto system_info = system_info_result.value();
  // get required extensions
  std::vector<const char*> extensions;
  u32 extensions_count;
  auto required_extensions =
    window_mgr.get_required_extensions(extensions_count).value();
  if (!system_info.is_extension_available(VK_EXT_DEBUG_REPORT_EXTENSION_NAME)) {
    logger.error(
      fmt::format("init failed, required extension not available: {}",
                  VK_EXT_DEBUG_REPORT_EXTENSION_NAME));
    return Status::ERROR;
  }
  extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  for (int i = 0; i < extensions_count; ++i) {
    if (!system_info.is_extension_available(required_extensions[i])) {
      logger.error(
        fmt::format("init failed, required extension not available: {}",
                    required_extensions[i]));
      return Status::ERROR;
    }
    extensions.push_back(required_extensions[i]);
  }
  // build instance
  vkb::InstanceBuilder builder;
  auto instance_result =
    builder.set_app_name(engine.name.c_str())
      .request_validation_layers(true) // todo@engine: debug only
      .use_default_debug_messenger()   // todo@engine: debug only
      .require_api_version(1, 3, 0)
      .enable_extensions(extensions)
      .build();
  if (!instance_result.has_value()) {
    logger.error(fmt::format("init failed, could not build instance: {}",
                             instance_result.full_error()));
    return Status::ERROR;
  }
  vkb::Instance instance = instance_result.value();
  instance_ = instance.instance;
  debug_messenger_ = instance.debug_messenger;
  // create surface
  surface_ = window_mgr.build_surface(instance_).value();
  // select physical device
  VkPhysicalDeviceVulkan12Features features_1d2 = {};
  features_1d2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
  features_1d2.bufferDeviceAddress = true;
  features_1d2.descriptorIndexing = true;
  VkPhysicalDeviceVulkan13Features features_1d3 = {};
  features_1d3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
  features_1d3.dynamicRendering = true;
  features_1d3.synchronization2 = true;
  vkb::PhysicalDeviceSelector selector(instance);
  auto selector_result = selector.set_minimum_version(1, 3)
                           .set_required_features_13(features_1d3)
                           .set_required_features_12(features_1d2)
                           .set_surface(surface_)
                           .select();
  if (!selector_result.has_value()) {
    logger.error(
      fmt::format("init failed, could not select physical device: {}",
                  selector_result.full_error()));
    return Status::ERROR;
  }
  auto vkb_phys_device = selector_result.value();
  gpu_ = vkb_phys_device.physical_device;
  // build device
  vkb::DeviceBuilder device_builder(vkb_phys_device);
  auto device_builder_result = device_builder.build();
  if (!device_builder_result.has_value()) {
    logger.error(fmt::format("init failed, could not build device: {}",
                             device_builder_result.full_error()));
    return Status::ERROR;
  }
  auto vkb_device = device_builder_result.value();
  device_ = vkb_device.device;
  // get graphics queue
  auto vkb_queue_index = vkb_device.get_queue_index(vkb::QueueType::graphics);
  if (!vkb_queue_index.has_value()) {
    logger.error(
      fmt::format("init failed, could not get graphics queue index: {}",
                  vkb_queue_index.full_error()));
    return Status::ERROR;
  }
  graphics_queue_family_ = vkb_queue_index.value();
  auto vkb_graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics);
  if (!vkb_graphics_queue.has_value()) {
    logger.error(fmt::format("init failed, could not get graphics queue: {}",
                             vkb_graphics_queue.full_error()));
    return Status::ERROR;
  }
  graphics_queue_ = vkb_graphics_queue.value();
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
  std::span<Descriptor::Allocator::PoolSizeRatio> sizes(sizes);
  descriptor_allocator_.init_pool(device_, 10, sizes);
  // setup destroyers
  del_queue_.push([&]() {
    descriptor_allocator_.destroy_pool(device_);

    vkDestroyDevice(device_, nullptr);
    vkb::destroy_debug_utils_messenger(instance_, debug_messenger_);
    vkDestroyInstance(instance_, nullptr);
    vmaDestroyAllocator(allocator_);
  });
  // success
  initialized = true;
  return Status::SUCCESS;
}

Manager::Manager(Engine* engine)
  : engine_{ engine } {};
