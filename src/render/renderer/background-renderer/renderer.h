#pragma once

#include "../renderer.h"

namespace render {

class BackgroundRenderer : Renderer
{
public:
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

    mgmt::vulkan::pipeline::Pipeline pipeline;
    ComputePushConstants data;
  };

  bool initialized = false;

  std::vector<ComputeEffect> effects_;
  u32 current_effect_ = 0;

  core::code init(mgmt::vulkan::Swapchain& swapchain);
  BackgroundRenderer(mgmt::vulkan::Manager* vk_mgr);

  core::code destroy();
  ~BackgroundRenderer();

  void draw(mgmt::vulkan::Swapchain& swapchain);
};

} // namespace render
