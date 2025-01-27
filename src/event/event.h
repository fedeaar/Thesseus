#pragma once

#include "../camera/camera.h"
#include "../core/types.h"
#include "../engine/engine.h"
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

class EventLoop;

class InputHandler
{
private:
  EventLoop* loop_;
  Engine* engine_;
  Camera* camera_;
  SDL_Event event_;
  const bool* keyboard_state_ = SDL_GetKeyboardState(NULL);
  bool first_mouse_ = true, mouse_lock_ = true;

  inline void handle_mouse_motion();

  inline void poll_events();
  inline void poll_keyboard();

public:
  InputHandler(EventLoop* loop, Engine* engine, Camera* camera);

  int init();

  void poll();
};

class EventLoop
{
  friend InputHandler;

private:
  Engine* engine_;
  Camera* camera_;
  InputHandler input_handler_;
  bool quit_ = false;
  f32 last_tick_ = 0.0f;

  int init();
  int destroy();

  void quit();

  inline f32 tick_delta();
  inline void tick();

public:
  EventLoop(Engine* engine, Camera* camera /*, Scene* scene*/);
  int run();
};
