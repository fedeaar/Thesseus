#include "manager.h"

//
// constructor
//

core::code
mgmt::WindowManager::init()
{
  if (initialized) {
    return core::code::SUCCESS;
  }
  // initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    core::Logger::err("mgmt::WindowManager::init",
                      "init failed to initialize SDL, {}",
                      SDL_GetError());
    return core::code::ERROR;
  }
  // create window
  u32 flags =
    (u32)(SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
  window_ =
    SDL_CreateWindow(window_name.c_str(), extent_.width, extent_.height, flags);
  if (window_ == NULL) {
    core::Logger::err("mgmt::WindowManager::init",
                      "init failed to create window, {}",
                      SDL_GetError());
    return core::code::ERROR;
  }
  // success
  initialized = true;
  return core::code::SUCCESS;
}

mgmt::WindowManager::WindowManager(u32 width,
                                   u32 height,
                                   std::string const& window_name)
  : width{ width }
  , height{ height }
  , extent_{ width, height }
  , aspect_ratio{ (f32)width / (f32)height }
  , window_name{ window_name } {};

//
// destructor
//

core::code
mgmt::WindowManager::destroy()
{
  if (!initialized) {
    core::Logger::err("mgmt::WindowManager::destroy",
                      "destroy called before initialization");
    return core::code::SUCCESS;
  }
  // destroy window
  SDL_DestroyWindow(window_);
  window_ = nullptr;
  // success
  initialized = false;
  return core::code::SUCCESS;
}

mgmt::WindowManager::~WindowManager()
{
  if (initialized) {
    destroy();
  }
};

//
// get
//

SDL_Window*
mgmt::WindowManager::get_window()
{
  return window_;
}

VkExtent2D&
mgmt::WindowManager::get_extent()
{
  return extent_;
}

core::Result<char const* const*, core::code>
mgmt::WindowManager::get_required_extensions(u32& count)
{
  if (!initialized) {
    core::Logger::err("mgmt::WindowManager::get_required_extensions",
                      "called before initialization");
    return core::code::NOT_INIT;
  }
  auto extensions = SDL_Vulkan_GetInstanceExtensions(&count);
  if (extensions == NULL) {
    core::Logger::err("mgmt::WindowManager::get_required_extensions",
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
mgmt::WindowManager::build_surface(VkInstance& instance, VkSurfaceKHR* surface)
{
  if (!initialized) {
    core::Logger::err("mgmt::WindowManager::build_surface",
                      "called before initialization");
    return core::code::NOT_INIT;
  }
  // create surface
  if (!SDL_Vulkan_CreateSurface(window_, instance, nullptr, surface)) {
    core::Logger::err("mgmt::WindowManager::build_surface",
                      "failed to create surface, {}",
                      SDL_GetError());
    return core::code::ERROR;
  }
  return core::code::SUCCESS;
}
