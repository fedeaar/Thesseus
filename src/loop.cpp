#include "loop.h"

//
// global
//

struct state* globalRef = NULL;
shaders::Default3d program1;
shaders::Default3dTextured program2;
Camera camera;
Model<shaders::Default3d> cube1 = models::cube::create_default3d_model();
Model<shaders::Default3dTextured> cube2 =
    models::cube::create_default3dTextured_model();
Texture2D texture("./res/container.jpg");
std::vector<texture_param> params = {{GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT},
                                     {GL_TEXTURE_WRAP_T, GL_REPEAT},
                                     {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
                                     {GL_TEXTURE_MAG_FILTER, GL_LINEAR}};

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
  program1.load();
  program2.load();
  texture.load(params);
  program2.set_texture(0);
  cube2.place({5, 2, 0});
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
  return true;
}

void loop::close() {
  texture.destroy();
  program1.destroy();
  program2.destroy();
}

void loop::render() {
  camera.set_speed(0.25f * (1 - delta()));
  handlePress();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  texture.bind(GL_TEXTURE0);
  program1.use();
  program1.set_view(camera.view_matrix());
  program1.set_projection(camera.proj_matrix((f32)globalRef->screen_width /
                                             (f32)globalRef->screen_height));
  cube1.render(program1);
  program2.use();
  program2.set_view(camera.view_matrix());
  program2.set_projection(camera.proj_matrix((f32)globalRef->screen_width /
                                             (f32)globalRef->screen_height));
  cube2.render(program2);
}
