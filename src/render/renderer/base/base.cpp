#pragma once

#include "base.h"

core::Status
render::Renderer::init()
{
  logger_.err("init error, base renderer is abstract");
  return core::Status::NOT_IMPLEMENTED;
};

render::Renderer::Renderer(mgmt::vulkan::Manager* vk_mgr)
  : vk_mgr_{ vk_mgr } {};

core::Status
render::Renderer::destroy()
{
  logger_.err("destroy error, base renderer is abstract");
  return core::Status::NOT_IMPLEMENTED;
};

render::Renderer::~Renderer() {};

core::Status
render::Renderer::draw(VkCommandBuffer cmd,
                       mgmt::vulkan::swapchain::Swapchain& swapchain)
{
  logger_.err("draw error, base renderer is abstract");
  return core::Status::NOT_IMPLEMENTED;
};
