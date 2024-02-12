#include "context.h"

//
// init
//

bool context::init(state* state) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Failed to initialize: %s\n", SDL_GetError());
    return false;
  }

  // Use OpenGL 3.3 core
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  // Create window
  state->window = SDL_CreateWindow(state->name.c_str(), state->screen_width,
                                   state->screen_height,
                                   SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED);
  if (state->window == NULL) {
    printf("Failed to create window: %s\n", SDL_GetError());
    return false;
  }

  // Create context
  state->gl_context = SDL_GL_CreateContext(state->window);
  if (state->gl_context == NULL) {
    printf("Failed to create OpenGl context: %s\n", SDL_GetError());
    return false;
  }

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK) {
    printf("Failed to initialze Glew. GLEW Error: %s\n",
           glewGetErrorString(glewError));
    return false;
  }

  // Use Vsync
  if (SDL_GL_SetSwapInterval(-1) < 0) {
    printf("Warning. Unable to set VSync: %s\n", SDL_GetError());
  }

  // set viewport
  glViewport(0, 0, state->screen_width, state->screen_height);

  // set clear color
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  // set depth testing
  glEnable(GL_DEPTH_TEST);

  return true;
}

//
// close
//

void context::close(struct state* state) {
  SDL_DestroyWindow(state->window);
  state->window = NULL;
  SDL_Quit();
}
