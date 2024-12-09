#include "engine.h"

RenderEngine::RenderEngine(RenderParams& params)
    : params_(params),
      aspect_ratio_((f32)params_.screen_width / (f32)params_.screen_height) {}

i32 RenderEngine::init() {
  // Use OpenGL 3.3 core
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  // Create window
  window_ = SDL_CreateWindow(params_.name.c_str(), params_.screen_width,
                             params_.screen_height,
                             SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED);
  if (window_ == NULL) {
    printf("Failed to create window: %s\n", SDL_GetError());
    return 0;
  }
  // Create context
  gl_context_ = SDL_GL_CreateContext(window_);
  if (gl_context_ == NULL) {
    printf("Failed to create OpenGl context: %s\n", SDL_GetError());
    return 0;
  }
  // Initialize GLEW
  glewExperimental = GL_TRUE;
  GLenum glew_error = glewInit();
  if (glew_error != GLEW_OK) {
    printf("Failed to initialze Glew. GLEW Error: %s\n",
           glewGetErrorString(glew_error));
    return 0;
  }
  // Use Vsync
  if (SDL_GL_SetSwapInterval(-1) < 0) {
    printf("Warning. Unable to set VSync: %s\n", SDL_GetError());
  }
  // set viewport
  glViewport(0, 0, params_.screen_width, params_.screen_height);
  // set clear color
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  // set depth testing
  glEnable(GL_DEPTH_TEST);

  return 1;
}

i32 RenderEngine::destroy() {
  SDL_DestroyWindow(window_);
  window_ = NULL;
};

void RenderEngine::render(const Camera& camera, Scene& scene) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  scene.render(camera);
  SDL_GL_SwapWindow(window_);
}

f32 RenderEngine::get_aspect_ratio() { return aspect_ratio_; }
