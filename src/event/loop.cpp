#include "event.h"

//
// private
//

inline f32 EventLoop::tick_delta() {
  f32 tick = SDL_GetTicks();
  f32 delta = (tick - last_tick_) / tick;
  last_tick_ = tick;
  return delta;
}

i32 EventLoop::init() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Failed to initialize: %s\n", SDL_GetError());
    return 0;
  }
  // TODO: proper error handling
  if (!engine_->init()) {
    return 0;
  }
  if (!input_handler_.init()) {
    return 0;
  }
  if (!scene_->init()) {
    return 0;
  }
  return 1;
}

i32 EventLoop::destroy() {
  // TODO: proper error handling
  if (!engine_->destroy()) {
    return 0;
  }
  if (!scene_->destroy()) {
    return 0;
  }
  SDL_Quit();
  return 1;
}

void EventLoop::quit() { quit_ = true; };

inline void EventLoop::tick() {
  input_handler_.poll();
  camera_->set_frame_delta(
      tick_delta());  // TODO: camera should handle this (?)
  engine_->render(*camera_, *scene_);
}

//
// public
//

EventLoop::EventLoop(RenderEngine* engine, Camera* camera, Scene* scene)
    : engine_(engine),
      camera_(camera),
      input_handler_(this, engine, camera),
      scene_(scene) {};

i32 EventLoop::run() {
  // TODO: proper error handling
  if (!init()) {
    return destroy();
  }
  while (!quit_) {
    tick();
  }
  return destroy();
}
