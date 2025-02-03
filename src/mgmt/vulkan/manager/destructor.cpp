#include "manager.h"

core::Status
mgmt::vulkan::Manager::destroy()
{
  if (!initialized) {
    logger.err("destroy failed, called before initialization");
    return core::Status::SUCCESS;
  }
  vkDeviceWaitIdle(device_);
  del_queue_.flush();
  initialized = false;
  return core::Status::SUCCESS;
};

mgmt::vulkan::Manager::~Manager()
{
  if (initialized) {
    destroy();
  }
};
