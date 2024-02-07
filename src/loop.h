#ifndef LOOP_
#define LOOP_

#include <SDL2/SDL.h>

#include <vector>

#include "camera/camera.h"
#include "context.h"
#include "core/inline.hpp"
#include "model/collection/collection.h"
#include "shader/collection/collection.h"

namespace loop {

bool init(struct state* global);
void close();
void handle(const SDL_Event& e);
void render();

}  // namespace loop

#endif  // LOOP_
