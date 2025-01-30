#pragma once

#include "../camera/camera.h"
#include "../core/types.h"
#include "../render-engine/engine/engine.h"

#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

class EventLoop;

class InputHandler
{
private:
  EventLoop* loop_;
  RenderEngine::Engine* engine_;
  Camera* camera_;
  SDL_Event event_;
  const bool* keyboard_state_ = SDL_GetKeyboardState(NULL);
  bool first_mouse_ = true, mouse_lock_ = true;

  inline void handle_mouse_motion();

  inline void poll_events();
  inline void poll_keyboard();

public:
  int init();
  InputHandler(EventLoop* loop, RenderEngine::Engine* engine, Camera* camera);

  void poll();
};

class EventLoop
{
  friend InputHandler;

private:
  RenderEngine::Engine* engine_;
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
  EventLoop(RenderEngine::Engine* engine, Camera* camera /*, Scene* scene*/);
  int run();
};
