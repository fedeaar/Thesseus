#include "event.h"

//
// constructor
//

core::Status
InputHandler::init()
{
  if (mouse_lock_) {
    SDL_HideCursor();
  } else {
    SDL_ShowCursor();
  }
  SDL_SetWindowRelativeMouseMode(engine_->get_window_mgr()->get_window(),
                                 mouse_lock_);
  return core::Status::SUCCESS;
}

InputHandler::InputHandler(EventLoop* loop,
                           render::Engine* engine,
                           Camera* camera)
  : loop_{ loop }
  , engine_{ engine }
  , camera_{ camera } {};

//
// handlers
//

inline void
InputHandler::handle_mouse_motion()
{
  // TODO: this should be handled by the camera (?)
  if (first_mouse_) {
    first_mouse_ = false;
    return;
  }
  if (!mouse_lock_) {
    return;
  }
  camera_->rotate(Camera::YAW, event_.motion.xrel);
  camera_->rotate(Camera::PITCH, event_.motion.yrel);
}

inline void
InputHandler::poll_events()
{
  while (SDL_PollEvent(&event_) != 0) {
    switch (event_.type) {
      case SDL_EVENT_QUIT:
        loop_->quit();
        break;
      case SDL_EVENT_MOUSE_MOTION:
        handle_mouse_motion();
        break;
    }
    ImGui_ImplSDL3_ProcessEvent(&event_);
  };
}

inline void
InputHandler::poll_keyboard()
{
  // TODO: this should be handled by the camera (?)
  if (keyboard_state_[SDL_SCANCODE_W]) {
    camera_->move(Camera::TOWARDS);
  }
  if (keyboard_state_[SDL_SCANCODE_S]) {
    camera_->move(Camera::AGAINST);
  }
  if (keyboard_state_[SDL_SCANCODE_SPACE]) {
    camera_->move(Camera::UPWARDS);
  }
  if (keyboard_state_[SDL_SCANCODE_LSHIFT]) {
    camera_->move(Camera::DOWNWARDS);
  }
  if (keyboard_state_[SDL_SCANCODE_A]) {
    camera_->move(Camera::LEFT);
  }
  if (keyboard_state_[SDL_SCANCODE_D]) {
    camera_->move(Camera::RIGHT);
  }
  if (keyboard_state_[SDL_SCANCODE_LALT] && keyboard_state_[SDL_SCANCODE_W]) {
    loop_->quit();
  }
  if (keyboard_state_[SDL_SCANCODE_ESCAPE]) {
    mouse_lock_ = !mouse_lock_;
    init(); // FIXME
  }
}

void
InputHandler::poll()
{
  poll_events();
  poll_keyboard();
}
