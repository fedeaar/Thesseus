#include "loop.h"

//
// global
//

struct state* globalRef = NULL;
Shader program;
Camera camera;
Texture2D texture1("./res/tex/container.jpg");
Texture2D texture2("./res/tex/awesome.png");
std::vector<texture_param> params = {
  {    GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT},
  {    GL_TEXTURE_WRAP_T,          GL_REPEAT},
  {GL_TEXTURE_MIN_FILTER,          GL_LINEAR},
  {GL_TEXTURE_MAG_FILTER,          GL_LINEAR}
};
Model<models::cube::data> cube = models::cube::create();

f32 intensity = 0.5;
f32 last_tick = 0.0f;
const u8* state = SDL_GetKeyboardState(NULL);
bool first_mouse = true;
bool mouse_lock = true;

inline f32 delta() {
  f32 tick = SDL_GetTicks();
  f32 delta = (tick - last_tick) / tick;
  last_tick = tick;
  return delta;
}

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
  program.load("./res/shaders/main.vs", "./res/shaders/main.fs");
  texture1.load(params);
  texture2.load(params);
  program.use();
  program.set_uniform("texture1", 0);
  program.set_uniform("texture2", 1);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
  return true;
}

void loop::close() {
  texture1.destroy();
  texture2.destroy();
  program.destroy();
}

void loop::render() {
  camera.set_speed(0.25f * (1 - delta()));
  handlePress();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  texture1.bind(GL_TEXTURE0);
  texture2.bind(GL_TEXTURE1);
  program.use();
  program.set_uniform("mix_intensity", clamp(intensity, 0.0f, 1.0f));
  program.set_uniform("view", camera.view_matrix());
  program.set_uniform(
    "projection",
    camera.proj_matrix(
      (f32)globalRef->screen_width / (f32)globalRef->screen_height));
  cube.render(program);
}
