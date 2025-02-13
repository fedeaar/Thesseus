#pragma once

#include "../camera/camera.h"
#include "../core/destructor-queue.h"
#include "../core/logger.hpp"
#include "../core/result.hpp"
#include "../core/status.h"
#include "../core/types.h"
#include "../mgmt/vulkan/descriptors/descriptors.h"
#include "../mgmt/vulkan/image/image.h"
#include "../mgmt/vulkan/info/info.h"
#include "../mgmt/vulkan/manager/manager.h"
#include "../mgmt/vulkan/mesh/mesh.h"
#include "../mgmt/vulkan/pipeline/pipeline.h"
#include "../mgmt/vulkan/swapchain/swapchain.h"
#include "../mgmt/window/manager.h"

#include <vulkan/vulkan.h>

#include <array>
#include <deque>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace render {

extern std::string const namespace_;

} // namespace render
