#pragma once

#include "../renderer.h"

namespace render {

class BackgroundRenderer : Renderer
{
private:
  struct GPUPushConstants
  {
    v4f data1;
    v4f data2;
    v4f data3;
    v4f data4;
  };
  struct ComputeEffect
  {
    const char* name;
    mgmt::vulkan::pipeline::Pipeline pipeline;
    GPUPushConstants data;
  };

public:
  core::status initialized = core::status::NOT_INITIALIZED;
  // state
  std::vector<ComputeEffect> effects_;
  u32 currentEffect_ = 0;

  core::code init();
  BackgroundRenderer(mgmt::vulkan::Swapchain* mp_swapchain,
                     mgmt::vulkan::Manager* mp_vkMgr);

  core::code destroy();
  ~BackgroundRenderer();

  void draw();
};

} // namespace render
