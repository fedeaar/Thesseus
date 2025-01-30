#pragma once

#include "../core/destructor-queue.h"
#include "../core/logger.h"
#include "../core/result.hpp"
#include "../core/types.h"
#include "../mgmt/vulkan-mgr/image/image.h"
#include "../mgmt/vulkan-mgr/info/info.h"
#include "../mgmt/vulkan-mgr/manager/manager.h"
#include "../mgmt/vulkan-mgr/pipeline/pipeline.h"
#include "../mgmt/vulkan-mgr/swapchain/swapchain.h"
#include "../mgmt/window-mgr/window-manager.h"

#include <vulkan/vulkan.h>

#include <array>
#include <deque>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace RenderEngine {

extern std::string const namespace_;

enum Status
{
  NOT_INIT = -2,
  ERROR = -1,
  SUCCESS = 1
};

} // namespace RenderEngine
