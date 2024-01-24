#ifndef LOOP_
#define LOOP_

#include "camera.h"
#include "context.h"
#include "core/inline.hpp"
#include "model.hpp"
#include "model/basic.h"
#include "shader.h"
#include "texture.h"
#include "vao.hpp"
#include <SDL2/SDL.h>
#include <vector>

namespace loop {

bool init(struct state* global);
void close();
void handle(const SDL_Event& e);
void render();

} // namespace loop

#endif // LOOP_
