#pragma once

#include "../camera/camera.h"
#include "../core/include.h"
#include "../render/include.h"

#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

class EventLoop;

class InputHandler
{
private:
  EventLoop* loop_;
  render::Engine* engine_;
  Camera* camera_;
  SDL_Event event_;
  const bool* keyboard_state_ = SDL_GetKeyboardState(NULL);
  bool first_mouse_ = true, mouse_lock_ = true;

  inline void handle_mouse_motion();

  inline void poll_events();
  inline void poll_keyboard();

public:
  core::code init();
  InputHandler(EventLoop* loop, render::Engine* engine, Camera* camera);

  void poll();
};

class EventLoop
{
  friend InputHandler;

private:
  render::Engine* engine_;
  Camera* camera_;
  InputHandler input_handler_;
  bool quit_ = false;
  f32 last_tick_ = 0.0f;

  core::code init();
  core::code destroy();

  void quit();

  inline f32 tick_delta();
  inline void tick();

public:
  EventLoop(render::Engine* engine, Camera* camera);
  core::code run();
};
