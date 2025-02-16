#include "manager.h"

//
// constructor
//

core::code
mgmt::window::Manager::init()
{
  if (state.status == core::status::INIT) {
    return core::code::SUCCESS;
  }
  if (state.status == core::status::ERROR) {
    core::Logger::wrn("mgmt::window::Manager::init",
                      "manager is in error state");
    return core::code::IN_ERROR_STATE;
  }
  // initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    core::Logger::err("mgmt::window::Manager::init",
                      "failed to initialize SDL, {}",
                      SDL_GetError());
    state.status = core::status::ERROR;
    return core::code::ERROR;
  }
  // create window
  u32 flags =
    (u32)(SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
  window_ = SDL_CreateWindow(
    state.window_name.c_str(), state.extent.width, state.extent.height, flags);
  if (window_ == NULL) {
    core::Logger::err("mgmt::window::Manager::init",
                      "failed to create window, {}",
                      SDL_GetError());
    state.status = core::status::ERROR;
    return core::code::ERROR;
  }
  // success
  state.status = core::status::INIT;
  return core::code::SUCCESS;
}

mgmt::window::Manager::Manager(u32 width,
                               u32 height,
                               std::string const& window_name)
  : state{ .extent = { width, height },
           .aspect_ratio{ (f32)width / (f32)height },
           .window_name{ window_name } } {};

//
// destructor
//

core::code
mgmt::window::Manager::destroy()
{
  if (state.status == core::status::NOT_INIT) {
    core::Logger::wrn("mgmt::window::Manager::destroy",
                      "destroy called before initialization");
    return core::code::SUCCESS;
  }
  // destroy window
  SDL_DestroyWindow(window_);
  window_ = nullptr;
  // success
  state.status = core::status::NOT_INIT;
  return core::code::SUCCESS;
}

mgmt::window::Manager::~Manager()
{
  if (state.status != core::status::NOT_INIT) {
    destroy();
  }
};

//
// get
//

SDL_Window*
mgmt::window::Manager::get_window()
{
  return window_;
}

core::code
mgmt::window::Manager::get_extent(VkExtent2D& extent)
{
  if (state.status != core::status::INIT) {
    core::Logger::err("mgmt::window::Manager::get_extent",
                      "called before initialization");
    return core::code::NOT_INIT;
  }
  int w, h; // move
  SDL_GetWindowSize(window_, &w, &h);
  extent = { (u32)w, (u32)h };
  return core::code::SUCCESS;
}

mgmt::window::Manager::State&
mgmt::window::Manager::get_state()
{
  return state;
}

core::Result<char const* const*, core::code>
mgmt::window::Manager::get_required_extensions(u32& count)
{
  if (state.status != core::status::INIT) {
    core::Logger::err("mgmt::window::Manager::get_required_extensions",
                      "called before initialization");
    return core::code::NOT_INIT;
  }
  auto extensions = SDL_Vulkan_GetInstanceExtensions(&count);
  if (extensions == NULL) {
    core::Logger::err("mgmt::window::Manager::get_required_extensions",
                      "failed, error: {}",
                      SDL_GetError());
    return core::code::ERROR;
  }
  return extensions;
}

//
// build surface
//

core::code
mgmt::window::Manager::build_surface(VkInstance& instance,
                                     VkSurfaceKHR* surface)
{
  if (state.status != core::status::INIT) {
    core::Logger::err("mgmt::window::Manager::build_surface",
                      "called before initialization");
    return core::code::NOT_INIT;
  }
  // create surface
  if (!SDL_Vulkan_CreateSurface(window_, instance, nullptr, surface)) {
    core::Logger::err("mgmt::window::Manager::build_surface",
                      "failed to create surface, {}",
                      SDL_GetError());
    return core::code::ERROR;
  }
  return core::code::SUCCESS;
}
