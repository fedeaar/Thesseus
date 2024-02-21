#ifndef LOOP_
#define LOOP_

#include <SDL3/SDL.h>

#include "camera/camera.h"
#include "context.h"
#include "scene/scene.h"

namespace loop {

bool init(struct state* global);
void close();
void handle(const SDL_Event& e);
void render();

}  // namespace loop

#endif  // LOOP_
