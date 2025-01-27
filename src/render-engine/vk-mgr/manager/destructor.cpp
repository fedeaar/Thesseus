#include "manager.h"

using namespace RenderEngine;
using namespace RenderEngine::VulkanManager;

Status
Manager::destroy()
{
  if (!initialized) {
    logger.log("destroy failed, called before initialization");
    return Status::SUCCESS;
  }
  del_queue_.flush();
  initialized = false;
  return Status::SUCCESS;
};

Manager::~Manager()
{
  if (initialized) {
    destroy();
  }
};
