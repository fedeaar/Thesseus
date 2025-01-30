#include "engine.h"

RenderEngine::Status
RenderEngine::Engine::render()
{
  // we assume we are init
  u32 img_idx;
  // await and reset command buffer
  auto command_buffer_result =
    vk_mgr_.swapchain_begin_commands(frame, swapchain_, img_idx);
  if (!command_buffer_result.has_value()) {
    return RenderEngine::Status::ERROR; // todo@engine: error msg
  }
  auto cmd = command_buffer_result.value();
  // todo@engine: fixme
  swapchain_.draw_extent.width = swapchain_.draw_img.extent.width;
  swapchain_.draw_extent.height = swapchain_.draw_img.extent.height;
  // draw
  swap_renderer_.draw(cmd, img_idx, swapchain_);
  imgui_renderer_.draw(cmd, img_idx, swapchain_);
  auto status = vk_mgr_.swapchain_end_commands(cmd, frame, img_idx, swapchain_);
  if (status != ResourceManagement::Status::SUCCESS) {
    return RenderEngine::Status::ERROR; // todo@engine: log error message
  }
  frame++;
}
