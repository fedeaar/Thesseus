#ifndef ENGINE_
#define ENGINE_

#include <SDL3/SDL.h>

#include "../camera/camera.h"
#include "../core/types.h"
#include "../scene/scene.h"

struct RenderParams {
  u32 screen_width, screen_height;
  string name;
};

class RenderEngine {
  RenderParams params_;
  f32 aspect_ratio_;
  SDL_Window* window_ = NULL;
  SDL_GLContext gl_context_ = NULL;

 public:
  RenderEngine(RenderParams& params);

  i32 init();
  i32 destroy();

  void render(const Camera& camera, Scene& scene);

  f32 get_aspect_ratio();
};

#endif  // ENGINE_
