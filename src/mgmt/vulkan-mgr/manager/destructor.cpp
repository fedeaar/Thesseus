#include "manager.h"

ResourceManagement::Status
ResourceManagement::VulkanManager::Manager::destroy()
{
  if (!initialized) {
    logger.log("destroy failed, called before initialization");
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
