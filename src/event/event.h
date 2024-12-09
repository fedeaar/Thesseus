#ifndef EVENT_
#define EVENT_

#include <SDL3/SDL.h>

#include "../camera/camera.h"
#include "../core/types.h"
#include "../engine/engine.h"
#include "../scene/scene.h"

class EventLoop;

class InputHandler {
 private:
  EventLoop* loop_;
  RenderEngine* engine_;
  Camera* camera_;
  SDL_Event event_;
  const u8* keyboard_state_ = SDL_GetKeyboardState(NULL);
  bool first_mouse_ = true, mouse_lock_ = true;

  inline void handle_mouse_motion();

  inline void poll_events();
  inline void poll_keyboard();

 public:
  InputHandler(EventLoop* loop, RenderEngine* engine, Camera* camera);

  int init();

  void poll();
};

class EventLoop {
  friend InputHandler;

 private:
  RenderEngine* engine_;
  Camera* camera_;
  Scene* scene_;
  InputHandler input_handler_;
  bool quit_ = false;
  f32 last_tick_ = 0.0f;

  int init();
  int destroy();

  void quit();

  inline f32 tick_delta();
  inline void tick();

 public:
  EventLoop(RenderEngine* engine, Camera* camera, Scene* scene);
  int run();
};

#endif  // EVENT_
