#include "manager.h"

//
// constructor
//

core::Status
mgmt::WindowManager::init()
{
  if (initialized) {
    return core::Status::SUCCESS;
  }
  // initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    logger_.err("init failed to initialize SDL, {}", SDL_GetError());
    return core::Status::ERROR;
  }
  // create window
  u32 flags =
    (u32)(SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
  window_ =
    SDL_CreateWindow(window_name.c_str(), extent_.width, extent_.height, flags);
  if (window_ == NULL) {
    logger_.err("init failed to create window, {}", SDL_GetError());
    return core::Status::ERROR;
  }
  // success
  initialized = true;
  return core::Status::SUCCESS;
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

core::Status
mgmt::WindowManager::destroy()
{
  if (!initialized) {
    logger_.err("destroy called before initialization");
    return core::Status::SUCCESS;
  }
  // destroy window
  SDL_DestroyWindow(window_);
  window_ = nullptr;
  // success
  initialized = false;
  return core::Status::SUCCESS;
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

core::Result<char const* const*, core::Status>
mgmt::WindowManager::get_required_extensions(u32& count)
{
  if (!initialized) {
    logger_.err("get_required_extensions called before initialization");
    return core::Status::NOT_INIT;
  }
  auto extensions = SDL_Vulkan_GetInstanceExtensions(&count);
  if (extensions == NULL) {
    logger_.err("get_required_extensions failed, error: {}", SDL_GetError());
    return core::Status::ERROR;
  }
  return extensions;
}

//
// build surface
//

core::Status
mgmt::WindowManager::build_surface(VkInstance& instance, VkSurfaceKHR* surface)
{
  if (!initialized) {
    logger_.err("build_surface called before initialization");
    return core::Status::NOT_INIT;
  }
  // create surface
  if (!SDL_Vulkan_CreateSurface(window_, instance, nullptr, surface)) {
    logger_.err("build_surface failed to create surface, {}", SDL_GetError());
    return core::Status::ERROR;
  }
  return core::Status::SUCCESS;
}
