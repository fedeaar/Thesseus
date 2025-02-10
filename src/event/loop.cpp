#include "event.h"

//
// constructor
//

core::Status
EventLoop::init()
{
  // TODO: proper error handling
  if (engine_->init() != core::Status::SUCCESS) {
    return core::Status::ERROR;
  }
  if (!input_handler_.init()) {
    return core::Status::ERROR;
  }
  return core::Status::SUCCESS;
}

EventLoop::EventLoop(render::Engine* engine, Camera* camera)
  : engine_{ engine }
  , camera_{ camera }
  , input_handler_{ this, engine, camera } {};

//
// destructor
//

core::Status
EventLoop::destroy()
{
  // TODO: proper error handling
  if (!engine_->destroy()) {
    return core::Status::ERROR;
  }
  SDL_Quit(); // TODO: this should not be here
  return core::Status::SUCCESS;
}

//
// run
//

void
EventLoop::quit()
{
  quit_ = true;
};

inline f32
EventLoop::tick_delta()
{
  f32 tick = SDL_GetTicks();
  f32 delta = (tick - last_tick_) / tick;
  last_tick_ = tick;
  return delta;
}

inline void
EventLoop::tick()
{
  engine_->maybe_resize_swapchain(); // fixme
  input_handler_.poll();
  // camera_->set_frame_delta(tick_delta()); // TODO: camera should handle this
  // (?) todo: move
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
  if (ImGui::Begin("background")) {
    ImGui::SliderFloat("Render Scale", &engine_->render_scale, 0.3f, 1.f);
    auto& selected =
      engine_->swap_renderer_.effects_[engine_->swap_renderer_.current_effect_];
    ImGui::Text("Selected effect: ", selected.name);
    ImGui::SliderInt("Effect Index",
                     (i32*)&engine_->swap_renderer_.current_effect_,
                     0,
                     engine_->swap_renderer_.effects_.size() - 1);
    ImGui::InputFloat4("data1", (float*)&selected.data.data1);
    ImGui::InputFloat4("data2", (float*)&selected.data.data2);
    ImGui::InputFloat4("data3", (float*)&selected.data.data3);
    ImGui::InputFloat4("data4", (float*)&selected.data.data4);
  }
  ImGui::End();
  ImGui::Render();
  engine_->render(*camera_);
}

core::Status
EventLoop::run()
{
  // TODO: proper error handling
  if (!init()) {
    return destroy();
  }
  while (!quit_) {
    tick();
  }
  return destroy();
}
