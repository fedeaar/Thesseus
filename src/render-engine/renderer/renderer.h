#pragma once

#include "render-engine.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <fmt/base.h>
#include <fmt/format.h>
#include <stdio.h>
#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#define RENDERER_LOG(x, flush)                                                 \
  fmt::print(x);                                                               \
  if (flush) {                                                                 \
    std::fflush(stdout);                                                       \
  }

#define RENDERER_ERROR(x)                                                      \
  RENDERER_LOG(fmt::format("RendererError: {}\n", x), true);

#define RENDERER_CHECK(x)                                                      \
  VkResult err = x;                                                            \
  if (err) {                                                                   \
    RENDERER_ERROR(string_VkResult(err));                                      \
    abort();                                                                   \
  }

namespace RenderEngine {

class Renderer
{
  friend Engine;
  friend VulkanManager;
  friend Swapchain;

public:
  bool initialized;
  enum ResultStatus
  {
    ERROR = 0,
    SUCCESS = 1,
  };

protected:
  Engine* engine_;
  VulkanManager* manager_;

public:
  Renderer(Engine* engine, VulkanManager* manager);
  ~Renderer();

  ResultStatus init();
  ResultStatus destroy();

  ResultStatus create_pipeline();
};
} // namespace RenderEngine
