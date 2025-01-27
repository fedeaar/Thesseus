#pragma once

#include "../core/result.hpp"
#include "../core/types.h"
#include "engine.h"
#include "swap-chain.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <fmt/base.h>
#include <fmt/format.h>
#include <stdio.h>
#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#define IMMSUBMIT_LOG(x, flush)                                                \
  fmt::print(x);                                                               \
  if (flush) {                                                                 \
    std::fflush(stdout);                                                       \
  }

#define IMMSUBMIT_ERROR(x)                                                     \
  IMMSUBMIT_LOG(fmt::format("ImmediateSubmitError: {}\n", x), true);

namespace RenderEngine {

struct ImmediateSubmitParams
{
  VkFence fence_;
  VkCommandBuffer command_buffer_;
  VkCommandPool command_pool_;
};

class ImmediateSubmit
{
  friend Engine;
  friend WindowManager;
  friend VulkanManager;

public:
  bool initialized;
  enum ResultStatus
  {
    ERROR = 0,
    SUCCESS = 1,
  };

private:
  VulkanManager* mgr_;
  ImmediateSubmitParams params_;

public:
  ImmediateSubmit(VulkanManager* manager, ImmediateSubmitParams const& params);
  ~ImmediateSubmit();

  void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);
};
} // namespace RenderEngine

//
// void
// Engine::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function)
// {
//   VK_CHECK(vkResetFences(device_, 1, &_immFence));
//   VK_CHECK(vkResetCommandBuffer(_immCommandBuffer, 0));
//
//   VkCommandBuffer cmd = _immCommandBuffer;
//
//   VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(
//     VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
//
//   VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
//
//   function(cmd);
//
//   VK_CHECK(vkEndCommandBuffer(cmd));
//
//   VkCommandBufferSubmitInfo cmdinfo =
//   vkinit::command_buffer_submit_info(cmd); VkSubmitInfo2 submit =
//   vkinit::submit_info(&cmdinfo, nullptr, nullptr);
//
//   // submit command buffer to the queue and execute it.
//   //  _renderFence will now block until the graphic commands finish execution
//   VK_CHECK(vkQueueSubmit2(graphics_queue_, 1, &submit, _immFence));
//
//   VK_CHECK(vkWaitForFences(device_, 1, &_immFence, true, 9999999999));
// }
//