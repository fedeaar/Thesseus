// #include "manager.h"
//
// using namespace ResourceManagement;
// using namespace ResourceManagement::VulkanManager;
//
// core::Result<ImmediateSubmit, Status>
// Manager::create_immediate_submit()
// {
//   if (!initialized) {
//     VKMGR_ERROR("vulkan manager is not initialized");
//     return Status::NOT_INIT;
//   }
//   ImmediateSubmitParams params;
//   auto command_pool_info = vkinit::command_pool_create_info(
//     graphics_queue_family_, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
//   VKMGR_CHECK(vkCreateCommandPool(
//     device_, &command_pool_info, nullptr, &params.command_pool_));
//   // allocate the command buffer for immediate submits
//   auto command_alloc_info =
//     vkinit::command_buffer_allocate_info(params.command_pool_, 1);
//   VKMGR_CHECK(vkAllocateCommandBuffers(
//     device_, &command_alloc_info, &params.command_buffer_));
//   // create fence
//   VkFenceCreateInfo fence_info =
//     vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
//   VKMGR_CHECK(vkCreateFence(device_, &fence_info, nullptr, &params.fence_));
//   // set destroyer
//   del_queue_.push(
//     [=]() { // todo@engine: should each component have its own queue?
//       vkDestroyFence(device_, params.fence_, nullptr);
//       vkDestroyCommandPool(device_, params.command_pool_, nullptr);
//     });
//   return ImmediateSubmit(this, params);
// }
