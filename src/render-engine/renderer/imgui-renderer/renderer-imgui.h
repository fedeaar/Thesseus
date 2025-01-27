#pragma once

#include "../core/result.hpp"
#include "../core/types.h"
#include "engine.h"
#include "immediate-submit.h"
#include "pipeline.h"
#include "renderer.h"
#include "swap-chain.h"
#include "vulkan-manager.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <fmt/base.h>
#include <fmt/format.h>
#include <stdio.h>
#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

namespace RenderEngine {

class ImguiRenderer : Renderer
{
  friend Engine;
  friend VulkanManager;
  friend Swapchain;

public:
  std::vector<ComputeEffect> effects_;

  ResultStatus init();
  ResultStatus draw(VkCommandBuffer cmd, VkImageView target);
};
} // namespace RenderEngine
