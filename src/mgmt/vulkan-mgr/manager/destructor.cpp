#include "manager.h"

ResourceManagement::Status
ResourceManagement::VulkanManager::Manager::destroy()
{
  logger.error("manager will be destroyed");
  if (!initialized) {
    logger.error("destroy failed, called before initialization");
    return Status::SUCCESS;
  }
  vkDeviceWaitIdle(device_);
  del_queue_.flush();
  SDL_DestroyWindow(window_);
  initialized = false;
  return Status::SUCCESS;
};

ResourceManagement::VulkanManager::Manager::~Manager()
{
  if (initialized) {
    destroy();
  }
};
