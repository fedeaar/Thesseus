#pragma once

#include "../renderer.h"

namespace render {

class SwapRenderer : Renderer
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

    mgmt::vulkan::Pipeline::Pipeline pipeline;
    ComputePushConstants data;
  };

  bool initialized = false;

  std::vector<ComputeEffect> effects_;
  u32 current_effect_;

  core::Status init(mgmt::vulkan::Swapchain::Swapchain& swapchain);
  SwapRenderer(mgmt::vulkan::Manager* vk_mgr);

  core::Status destroy();
  ~SwapRenderer();

  core::Status draw(VkCommandBuffer cmd,
                    u32 img_idx,
                    mgmt::vulkan::Swapchain::Swapchain& swapchain);
};
} // namespace render
