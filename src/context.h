#ifndef CONTEXT_
#define CONTEXT_

#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <string>

struct state
{
  uint screen_width, screen_height;
  std::string name;

  SDL_Window* window = NULL;
  SDL_GLContext gl_context = NULL;

  bool quit = false;
};

namespace context {

bool init(struct state* state);
void close(struct state* state);

} // namespace context

#endif // CONTEXT_
