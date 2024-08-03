#include "loop.h"

//
// global
//

struct state* global_ref = NULL;
Camera camera;  // TODO: camera and 'player' handlers should be abstracted away
                // to allow swapping

//
// handlers
//

f32 last_tick = 0.0f;
inline f32 delta() {
  f32 tick = SDL_GetTicks();
  f32 delta = (tick - last_tick) / tick;
  last_tick = tick;
  return delta;
}

const u8* state = SDL_GetKeyboardState(NULL);
bool first_mouse = true;

void handle_press() {
  if (state[SDL_SCANCODE_W]) {
    camera.move(Camera::TOWARDS);
  }
  if (state[SDL_SCANCODE_S]) {
    camera.move(Camera::AGAINST);
  }
  if (state[SDL_SCANCODE_SPACE]) {
    camera.move(Camera::UPWARDS);
  }
  if (state[SDL_SCANCODE_LSHIFT]) {
    camera.move(Camera::DOWNWARDS);
  }
  if (state[SDL_SCANCODE_A]) {
    camera.move(Camera::LEFT);
  }
  if (state[SDL_SCANCODE_D]) {
    camera.move(Camera::RIGHT);
  }
  if (state[SDL_SCANCODE_LALT] && state[SDL_SCANCODE_W]) {
    global_ref->quit = true;
  }
}

bool mouse_lock = true;

void loop::handle(const SDL_Event& e) {
  switch (e.type) {
    case SDL_EVENT_MOUSE_MOTION:
      if (first_mouse) {
        first_mouse = false;
        break;
      }
      camera.rotate(Camera::YAW, e.motion.xrel);
      camera.rotate(Camera::PITCH, e.motion.yrel);
      break;
  }
}

bool loop::init(struct state* global) {
  global_ref = global;
  if (mouse_lock) {
    SDL_HideCursor();
  } else {
    SDL_ShowCursor();
  }
  SDL_SetRelativeMouseMode(mouse_lock ? SDL_TRUE : SDL_FALSE);
  f32 aspect_ratio =
      (f32)global_ref->screen_width / (f32)global_ref->screen_height;
  camera.set_aspect(aspect_ratio);
  scene::init();
  return true;
}

void loop::close() { scene::destroy(); }

void loop::render() {
  camera.set_speed(0.25f * (1 - delta()));
  handle_press();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  scene::render(camera);
}
