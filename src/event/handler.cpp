#include "event.h"

//
// constructor
//

core::code
InputHandler::init()
{
  if (mouseLock_) {
    SDL_HideCursor();
  } else {
    SDL_ShowCursor();
  }
  SDL_SetWindowRelativeMouseMode(p_engine_->state.windowMgr.get_window(),
                                 mouseLock_);
  return core::code::SUCCESS;
}

InputHandler::InputHandler(EventLoop* loop,
                           render::Engine* engine,
                           Camera* camera)
  : p_loop_{ loop }
  , p_engine_{ engine }
  , p_camera_{ camera } {};

//
// handlers
//

inline void
InputHandler::handle_mouse_motion()
{
  // TODO: this should be handled by the camera (?)
  if (firstMouse_) {
    firstMouse_ = false;
    return;
  }
  if (!mouseLock_) {
    return;
  }
  p_camera_->rotate(Camera::YAW, p_event_.motion.xrel);
  p_camera_->rotate(Camera::PITCH, p_event_.motion.yrel);
}

inline void
InputHandler::poll_events()
{
  while (SDL_PollEvent(&p_event_) != 0) {
    switch (p_event_.type) {
      case SDL_EVENT_QUIT:
        p_loop_->quit();
        break;
      case SDL_EVENT_MOUSE_MOTION:
        handle_mouse_motion();
        break;
    }
    ImGui_ImplSDL3_ProcessEvent(&p_event_);
  };
}

inline void
InputHandler::poll_keyboard()
{
  // TODO: this should be handled by the camera (?)
  if (p_keyboardState_[SDL_SCANCODE_W]) {
    p_camera_->move(Camera::TOWARDS);
  }
  if (p_keyboardState_[SDL_SCANCODE_S]) {
    p_camera_->move(Camera::AGAINST);
  }
  if (p_keyboardState_[SDL_SCANCODE_SPACE]) {
    p_camera_->move(Camera::UPWARDS);
  }
  if (p_keyboardState_[SDL_SCANCODE_LSHIFT]) {
    p_camera_->move(Camera::DOWNWARDS);
  }
  if (p_keyboardState_[SDL_SCANCODE_A]) {
    p_camera_->move(Camera::LEFT);
  }
  if (p_keyboardState_[SDL_SCANCODE_D]) {
    p_camera_->move(Camera::RIGHT);
  }
  if (p_keyboardState_[SDL_SCANCODE_LALT] && p_keyboardState_[SDL_SCANCODE_W]) {
    p_loop_->quit();
  }
  if (p_keyboardState_[SDL_SCANCODE_ESCAPE]) {
    mouseLock_ = !mouseLock_;
    init(); // FIXME
  }
}

void
InputHandler::poll()
{
  poll_events();
  poll_keyboard();
}
