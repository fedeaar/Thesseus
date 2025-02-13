#pragma once

#include "renderer.h"

core::code
render::Renderer::init()
{
  logger_.err("init error, base renderer is abstract");
  return core::code::NOT_IMPLEMENTED;
};

render::Renderer::Renderer(mgmt::vulkan::Manager* vk_mgr)
  : vk_mgr_{ vk_mgr } {};

core::code
render::Renderer::destroy()
{
  logger_.err("destroy error, base renderer is abstract");
  return core::code::NOT_IMPLEMENTED;
};

render::Renderer::~Renderer() {};

core::code
render::Renderer::draw(VkCommandBuffer cmd,
                       mgmt::vulkan::swapchain::Swapchain& swapchain)
{
  logger_.err("draw error, base renderer is abstract");
  return core::code::NOT_IMPLEMENTED;
};
