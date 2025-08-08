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
  EventLoop* p_loop_;
  render::Engine* p_engine_;
  Camera* p_camera_;
  SDL_Event p_event_;
  const bool* p_keyboardState_ = SDL_GetKeyboardState(NULL);
  bool firstMouse_ = true, mouseLock_ = true;

  inline void handle_mouse_motion();

  inline void poll_events();
  inline void poll_keyboard();

public:
  core::code init();
  InputHandler(EventLoop* p_loop, render::Engine* p_engine, Camera* p_camera);

  void poll();
};

class EventLoop
{
  friend InputHandler;

private:
  render::Engine* p_engine_;
  Camera* p_camera_;
  InputHandler p_inputHandler_;
  bool quit_ = false;
  f32 lastTick_ = 0.0f;
  debug::GlobalStats* p_stats_;

  core::code init();
  core::code destroy();

  void quit();

  inline f32 tick_delta();
  inline void tick();

public:
  EventLoop(render::Engine* p_engine,
            Camera* p_camera,
            debug::GlobalStats* p_stats);
  core::code run();
};
