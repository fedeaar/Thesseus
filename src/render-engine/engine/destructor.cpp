
// i32
// Engine::destroy()
// {
//   if (!initialized_) {
//     return 0;
//   }
//   vkDeviceWaitIdle(device_);

//   main_del_queue_.flush();
//   destroy_swapchain();
//
//   SDL_DestroyWindow(window_);
//   window_ = nullptr;
//   return 1;
// };