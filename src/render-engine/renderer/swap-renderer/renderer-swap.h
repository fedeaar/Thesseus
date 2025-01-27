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

struct ComputePushConstants
{
  glm::vec4 data1;
  glm::vec4 data2;
  glm::vec4 data3;
  glm::vec4 data4;
};

struct ComputeEffect
{
  const char* name;

  Pipeline pipeline;
  ComputePushConstants data;
};

class SwapRenderer : Renderer
{
  friend Engine;
  friend VulkanManager;
  friend Swapchain;

public:
  std::vector<ComputeEffect> effects_;
  u32 current_effect_;

  ResultStatus create_pipeline();

  ResultStatus draw(VkCommandBuffer cmd);
};
} // namespace RenderEngine
