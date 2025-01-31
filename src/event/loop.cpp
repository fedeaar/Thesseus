#include "event.h"

//
// private
//

inline f32
EventLoop::tick_delta()
{
  f32 tick = SDL_GetTicks();
  f32 delta = (tick - last_tick_) / tick;
  last_tick_ = tick;
  return delta;
}

i32
EventLoop::init()
{
  // TODO: proper error handling
  if (engine_->init() != ResourceManagement::Status::SUCCESS) {
    return 0;
  }
  if (!input_handler_.init()) {
    return 0;
  }
  // if (!scene_->init()) {
  //   return 0;
  // }
  return 1;
}

i32
EventLoop::destroy()
{
  // TODO: proper error handling
  if (!engine_->destroy()) {
    return 0;
  }
  // if (!scene_->destroy()) {
  //   return 0;
  // }
  SDL_Quit();
  return 1;
}

void
EventLoop::quit()
{
  quit_ = true;
};

inline void
EventLoop::tick()
{
  input_handler_.poll();
  camera_->set_frame_delta(tick_delta()); // TODO: camera should handle this (?)
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
  if (ImGui::Begin("background")) {
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
  engine_->render(/*camera_*, *scene_*/);
}

//
// public
//

EventLoop::EventLoop(RenderEngine::Engine* engine,
                     Camera* camera /*,  Scene* scene */)
  : engine_(engine)
  , camera_(camera)
  , input_handler_(this, engine, camera)
/*, scene_(scene) */ {};

i32
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
