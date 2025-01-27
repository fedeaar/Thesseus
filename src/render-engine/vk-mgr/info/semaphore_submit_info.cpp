#include "info.h"

VkSemaphoreSubmitInfo
RenderEngine::VulkanManager::Info::semaphore_submit_info(
  VkPipelineStageFlags2 stage_mask,
  VkSemaphore semaphore)
{
  VkSemaphoreSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
  submit_info.pNext = nullptr;
  submit_info.semaphore = semaphore;
  submit_info.stageMask = stage_mask;
  submit_info.deviceIndex = 0;
  submit_info.value = 1;
  return submit_info;
}
