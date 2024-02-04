#ifndef LOOP_
#define LOOP_

#include "camera.h"
#include "context.h"
#include "core/inline.hpp"
#include "model/cube/decl.h"
#include "shader/default-3d/decl.h"
#include <SDL2/SDL.h>
#include <vector>

namespace loop {

bool init(struct state* global);
void close();
void handle(const SDL_Event& e);
void render();

} // namespace loop

#endif // LOOP_
