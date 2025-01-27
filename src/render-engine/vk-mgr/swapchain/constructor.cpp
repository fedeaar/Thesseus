#include "swapchain.h"

using namespace RenderEngine;
using namespace RenderEngine::VulkanManager;

Swapchain::Swapchain(Manager* manager, Swapchain::Params const& params)
  : vk_mgr_{ manager }
  , params_{ params } {};
