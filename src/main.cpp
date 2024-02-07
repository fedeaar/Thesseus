#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "loop.h"

//
// global
//

state global{
    640,        // screen_width
    480,        // screen_height
    "Thesseus"  // name
};

//
// main
//

void close(int code) {
  loop::close();
  context::close(&global);
  exit(code);
}

int main(int argc, char* args[]) {
  if (!context::init(&global)) {
    close(1);
  }
  if (!loop::init(&global)) {
    close(1);
  }
  SDL_Event e;
  while (!global.quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        global.quit = true;
      } else {
        loop::handle(e);
      }
    }
    loop::render();
    SDL_GL_SwapWindow(global.window);
  }
  close(0);
}
