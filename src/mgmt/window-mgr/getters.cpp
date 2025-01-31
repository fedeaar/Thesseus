#include "window-manager.h"

SDL_Window*
ResourceManagement::WindowManager::get_window()
{
  return window_;
}

VkExtent2D&
ResourceManagement::WindowManager::get_extent()
{
  return extent_;
}
