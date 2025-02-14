#pragma once

#include "renderer.h"

core::code
render::Renderer::init()
{
  core::Logger::err("render::Renderer::init", "base renderer is abstract");
  return core::code::NOT_IMPLEMENTED;
};

render::Renderer::Renderer(mgmt::vulkan::Manager* vk_mgr)
  : vk_mgr_{ vk_mgr } {};

core::code
render::Renderer::destroy()
{
  core::Logger::err("render::Renderer::destroy", "base renderer is abstract");
  return core::code::NOT_IMPLEMENTED;
};

render::Renderer::~Renderer() {};

void
render::Renderer::draw(mgmt::vulkan::Swapchain& swapchain)
{
  core::Logger::err("render::Renderer::draw", "base renderer is abstract");
};
