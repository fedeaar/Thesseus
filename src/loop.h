#ifndef PLAYGROUND_
#define PLAYGROUND_

#include "camera.h"
#include "context.h"
#include "core/inline.hpp"
#include "model.hpp"
#include "shader.h"
#include "texture.h"
#include "vao.hpp"
#include <SDL2/SDL.h>

namespace loop {

bool init(struct state* global);
void close();
void handle(const SDL_Event& e);
void render();

} // namespace playground

#endif // PLAYGROUND_
