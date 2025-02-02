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
  initialized = false;
  return Status::SUCCESS;
};

ResourceManagement::VulkanManager::Manager::~Manager()
{
  if (initialized) {
    destroy();
  }
};
