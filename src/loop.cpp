#include "loop.h"
#include <vector>

//
// data
//

struct __attribute__((__packed__)) data {
  GLfloat x, y, z, s, t;
};

std::vector<attribute> attr = {
  {GL_FLOAT, 3, GL_FALSE},
  {GL_FLOAT, 2, GL_FALSE},
};

std::vector<data> buffer = {
  {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f},
  { 0.5f, -0.5f, -0.5f, 1.0f, 0.0f},
  { 0.5f,  0.5f, -0.5f, 1.0f, 1.0f},
  { 0.5f,  0.5f, -0.5f, 1.0f, 1.0f},
  {-0.5f,  0.5f, -0.5f, 0.0f, 1.0f},
  {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f},

  {-0.5f, -0.5f,  0.5f, 0.0f, 0.0f},
  { 0.5f, -0.5f,  0.5f, 1.0f, 0.0f},
  { 0.5f,  0.5f,  0.5f, 1.0f, 1.0f},
  { 0.5f,  0.5f,  0.5f, 1.0f, 1.0f},
  {-0.5f,  0.5f,  0.5f, 0.0f, 1.0f},
  {-0.5f, -0.5f,  0.5f, 0.0f, 0.0f},

  {-0.5f,  0.5f,  0.5f, 1.0f, 0.0f},
  {-0.5f,  0.5f, -0.5f, 1.0f, 1.0f},
  {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
  {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
  {-0.5f, -0.5f,  0.5f, 0.0f, 0.0f},
  {-0.5f,  0.5f,  0.5f, 1.0f, 0.0f},

  { 0.5f,  0.5f,  0.5f, 1.0f, 0.0f},
  { 0.5f,  0.5f, -0.5f, 1.0f, 1.0f},
  { 0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
  { 0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
  { 0.5f, -0.5f,  0.5f, 0.0f, 0.0f},
  { 0.5f,  0.5f,  0.5f, 1.0f, 0.0f},

  {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
  { 0.5f, -0.5f, -0.5f, 1.0f, 1.0f},
  { 0.5f, -0.5f,  0.5f, 1.0f, 0.0f},
  { 0.5f, -0.5f,  0.5f, 1.0f, 0.0f},
  {-0.5f, -0.5f,  0.5f, 0.0f, 0.0f},
  {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},

  {-0.5f,  0.5f, -0.5f, 0.0f, 1.0f},
  { 0.5f,  0.5f, -0.5f, 1.0f, 1.0f},
  { 0.5f,  0.5f,  0.5f, 1.0f, 0.0f},
  { 0.5f,  0.5f,  0.5f, 1.0f, 0.0f},
  {-0.5f,  0.5f,  0.5f, 0.0f, 0.0f},
  {-0.5f,  0.5f, -0.5f, 0.0f, 1.0f}
};

glm::vec3 cubePositions[] = {
  glm::vec3(0.0f, 0.0f, 0.0f),
  glm::vec3(2.0f, 5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f, 3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f, 2.0f, -2.5f),
  glm::vec3(1.5f, 0.2f, -1.5f),
  glm::vec3(-1.3f, 1.0f, -1.5f)};

//
// global
//

struct state* globalRef = NULL;
Shader program;
Camera camera;
VAO<data> vao(buffer, attr);
std::shared_ptr<VAO<data>> vao_ptr(&vao, [](VAO<data>*) {
}); // no need to delete as it is static
std::vector<Model<data>> models {};
Texture2D texture1("./res/tex/container.jpg");
Texture2D texture2("./res/tex/awesome.png");
std::vector<texture_param> params = {
  {    GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT},
  {    GL_TEXTURE_WRAP_T,          GL_REPEAT},
  {GL_TEXTURE_MIN_FILTER,          GL_LINEAR},
  {GL_TEXTURE_MAG_FILTER,          GL_LINEAR}
};
f32 intensity = 0.5;
f32 last_tick = 0.0f;
const u8* state = SDL_GetKeyboardState(NULL);
bool first_mouse = true;

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
}

//
// handlers
//

bool loop::init(struct state* global) {
  globalRef = global;
  SDL_ShowCursor(0);
  SDL_SetRelativeMouseMode(SDL_TRUE);
  program.load("./res/shaders/main.vs", "./res/shaders/main.fs");
  vao.load();
  texture1.load(params);
  texture2.load(params);
  program.use();
  program.set_uniform("texture1", 0);
  program.set_uniform("texture2", 1);
  for (auto& pos : cubePositions) {
    models.push_back(Model<data> {
      vao_ptr, pos, 0, {1.0f, 0.3f, 0.5f}
    });
  }
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
  return true;
}

void loop::close() {
  texture1.destroy();
  texture2.destroy();
  vao.destroy();
  program.destroy();
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
  for (unsigned int i = 0; i < 10; i++) {
    models[i].render(program);
  };
}
