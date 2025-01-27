#define VMA_IMPLEMENTATION
#include "engine.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

using namespace RenderEngine;

// Engine::Engine(EngineParams& params)
// {
//   params_ = params;
//   window_mgr_ = WindowManager(this);
//   vk_mgr_ = VulkanManager(this);
//   auto swapchain_result = vk_mgr_.create_swapchain();
//   if (!swapchain_result.has_value()) {
//     throw swapchain_result.error(); // todo@engine
//   }
//   swapchain_ = swapchain_result.value();
// }

// i32
// Engine::init()
// {
//   if (initialized) {
//     return 0;
//   }
//   if (!window_mgr_.init()) {
//     return 0;
//   }
//   if (!vulkan_mgr_.init()) {
//     return 0;
//   }
//   if (!init_swapchain()) {
//     fprintf(stderr, "Failed to initialize swapchain");
//     return 0;
//   }
//   if (!init_commands()) {
//     fprintf(stderr, "Failed to initialize commands");
//     return 0;
//   }
//   if (!init_sync_structures()) {
//     fprintf(stderr, "Failed to initialize sync structures");
//     return 0;
//   }
//   init_descriptors();
//   init_pipelines();
//   init_imgui();
//   initialized_ = true;
//   return 1;
// }
