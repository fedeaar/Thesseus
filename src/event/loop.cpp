#include "event.h"

//
// constructor
//

core::code
EventLoop::init()
{
  // TODO: proper error handling
  if (p_engine_->init() != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  if (p_inputHandler_.init() != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  return core::code::SUCCESS;
}

EventLoop::EventLoop(render::Engine* engine,
                     Camera* camera,
                     debug::GlobalStats* p_stats)
  : p_engine_{ engine }
  , p_camera_{ camera }
  , p_stats_(p_stats)
  , p_inputHandler_{ this, engine, camera } {};

//
// destructor
//

core::code
EventLoop::destroy()
{
  // TODO: proper error handling
  if (p_engine_->destroy() != core::code::SUCCESS) {
    return core::code::ERROR;
  }
  SDL_Quit(); // TODO: this should not be here
  return core::code::SUCCESS;
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
  f32 delta = (tick - lastTick_) / tick;
  lastTick_ = tick;
  return delta;
}

inline void
EventLoop::tick()
{
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  p_inputHandler_.poll();
  p_camera_->set_frame_delta(tick_delta());
  p_engine_->render();
}

core::code
EventLoop::run()
{
  // TODO: proper error handling
  if (init() != core::code::SUCCESS) {
    return destroy();
  }
  while (!quit_) {
    auto start = std::chrono::system_clock::now();
    tick();
    auto end = std::chrono::system_clock::now();
    auto elapsed =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    p_stats_->frametime = elapsed.count() / 1000.f;
  }
  return destroy();
}
