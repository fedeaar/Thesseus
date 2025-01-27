#pragma once

#include "../core/destructor-queue.h"
#include "../core/logger.h"
#include "../core/result.hpp"
#include "../core/types.h"
#include "./swapchain/swapchain.h"
#include "./vk-mgr/vulkan-manager.h"
#include "./window-mgr/window-manager.h"
// #include "./imm-submit/immediate-submit.h"
// #include "./renderer/renderer.h"
#include "./engine/engine.h"

#include <vulkan/vulkan.h>

#include <array>
#include <deque>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace RenderEngine {

static inline std::string const&
namespace_()
{
  return "RenderEngine";
}

enum Status
{
  NOT_INIT = -2,
  ERROR = -1,
  SUCCESS = 1
};

class WindowManager;
class VulkanManager;
class Renderer;

} // namespace RenderEngine
