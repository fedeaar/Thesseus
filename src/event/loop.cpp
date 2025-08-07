#include "event.h"

//
// constructor
//

core::code
EventLoop::init()
{
  // TODO: proper error handling
  if (engine_->init() != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  if (input_handler_.init() != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  return core::code::SUCCESS;
}

EventLoop::EventLoop(render::Engine* engine, Camera* camera)
  : engine_{ engine }
  , camera_{ camera }
  , input_handler_{ this, engine, camera } {};

//
// destructor
//

core::code
EventLoop::destroy()
{
  // TODO: proper error handling
  if (engine_->destroy() != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  SDL_Quit(); // TODO: this should not be here
  return core::code::SUCCESS;
}

//
// debug
//

void
update_debug_gui(render::Engine& engine)
{
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
  if (ImGui::Begin("background")) {
    ImGui::SliderFloat(
      "Render Scale", &engine.state.swapchain.render_scale, 0.3f, 1.f);
    auto& selected =
      engine.state.bgRenderer.effects_[engine.state.bgRenderer.currentEffect_];
    ImGui::Text("Selected effect: ", selected.name);
    ImGui::SliderInt("Effect Index",
                     (i32*)&engine.state.bgRenderer.currentEffect_,
                     0,
                     engine.state.bgRenderer.effects_.size() - 1);
    ImGui::InputFloat4("data1", (float*)&selected.data.data1);
    ImGui::InputFloat4("data2", (float*)&selected.data.data2);
    ImGui::InputFloat4("data3", (float*)&selected.data.data3);
    ImGui::InputFloat4("data4", (float*)&selected.data.data4);
  }
  ImGui::End();
  ImGui::Render();
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
  input_handler_.poll();
  update_debug_gui(*engine_);
  camera_->set_frame_delta(tick_delta());
  engine_->render();
}

core::code
EventLoop::run()
{
  // TODO: proper error handling
  if (init() != core::code::SUCCESS) {
    return destroy();
  }
  while (!quit_) {
    tick();
  }
  return destroy();
}
