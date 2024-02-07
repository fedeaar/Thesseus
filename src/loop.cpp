#include "loop.h"

//
// global
//

struct state* globalRef = NULL;
shaders::Default3d program{};
Camera camera;
Model<shaders::Default3d> cube = models::cube::create();

f32 last_tick = 0.0f;

inline f32 delta() {
  f32 tick = SDL_GetTicks();
  f32 delta = (tick - last_tick) / tick;
  last_tick = tick;
  return delta;
}

const u8* state = SDL_GetKeyboardState(NULL);
bool first_mouse = true;

void handlePress() {
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
    globalRef->quit = true;
  }
}

bool mouse_lock = true;

void loop::handle(const SDL_Event& e) {
  switch (e.type) {
    case SDL_MOUSEMOTION:
      if (first_mouse) {
        first_mouse = false;
        break;
      }
      camera.rotate(Camera::YAW, e.motion.xrel);
      camera.rotate(Camera::PITCH, e.motion.yrel);
      break;
  }
}

//
// handlers
//

bool loop::init(struct state* global) {
  globalRef = global;
  SDL_ShowCursor(!mouse_lock);
  SDL_SetRelativeMouseMode(mouse_lock ? SDL_TRUE : SDL_FALSE);
  program.load();
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
  return true;
}

void loop::close() { program.destroy(); }

void loop::render() {
  camera.set_speed(0.25f * (1 - delta()));
  handlePress();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  program.use();
  program.set_view(camera.view_matrix());
  program.set_projection(camera.proj_matrix((f32)globalRef->screen_width /
                                            (f32)globalRef->screen_height));
  cube.render(program);
}
