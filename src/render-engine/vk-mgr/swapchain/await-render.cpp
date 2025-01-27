/*
  vk_mgr_.await_fence(swapchain_.current_frame().render_fence);
  // wait and prepare for next render
  VK_CHECK(vkWaitForFences(device_,
                           1,
                           &get_current_frame().render_fence,
                           true,
                           1000000000)); // 1 sec
  get_current_frame().del_queue.flush();
  VK_CHECK(vkResetFences(device_, 1, &get_current_frame().render_fence));
  u32 img_idx;
  VK_CHECK(vkAcquireNextImageKHR(device_,
                                 swapchain_,
                                 1000000000,
                                 get_current_frame().swapchain_semaphore,
                                 nullptr,
                                 &img_idx));
  VkCommandBuffer cmd = get_current_frame().main_command_buffer;
  VK_CHECK(vkResetCommandBuffer(cmd, 0));
  VkCommandBufferBeginInfo cmd_info = vkinit::command_buffer_begin_info(
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_info));
  // draw
  draw_extent_.width = draw_img_.extent.width;
  draw_extent_.height = draw_img_.extent.height;
  vkutil::transition_image(
    cmd, draw_img_.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
  draw_background(cmd);
  // transition the draw image and the swapchain image into their correct
  // transfer layouts
  vkutil::transition_image(cmd,
                           draw_img_.image,
                           VK_IMAGE_LAYOUT_GENERAL,
                           VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  vkutil::transition_image(cmd,
                           swapchain_img_[img_idx],
                           VK_IMAGE_LAYOUT_UNDEFINED,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  vkutil::copy_image_to_image(cmd,
                              draw_img_.image,
                              swapchain_img_[img_idx],
                              draw_extent_,
                              swapchain_extent_);
  // set swapchain image layout to Attachment Optimal so we can draw it
  vkutil::transition_image(cmd,
                           swapchain_img_[img_idx],
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  // draw imgui into the swapchain image
  draw_imgui(cmd, swapchain_img_views_[img_idx]);

  vkutil::transition_image(cmd,
                           swapchain_img_[img_idx],
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  VK_CHECK(vkEndCommandBuffer(cmd));
  // prepare the submission to the queue
  VkCommandBufferSubmitInfo cmd_submit_info =
    vkinit::command_buffer_submit_info(cmd);
  VkSemaphoreSubmitInfo wait_info = vkinit::semaphore_submit_info(
    VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
    get_current_frame().swapchain_semaphore);
  VkSemaphoreSubmitInfo signal_info = vkinit::semaphore_submit_info(
    VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, get_current_frame().render_semaphore);
  VkSubmitInfo2 submit =
    vkinit::submit_info(&cmd_submit_info, &signal_info, &wait_info);
  // submit
  VK_CHECK(vkQueueSubmit2(
    graphics_queue_, 1, &submit, get_current_frame().render_fence));
  // display
  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.pNext = nullptr;
  present_info.pSwapchains = &swapchain_;
  present_info.swapchainCount = 1;
  present_info.pWaitSemaphores = &get_current_frame().render_semaphore;
  present_info.waitSemaphoreCount = 1;
  present_info.pImageIndices = &img_idx;
  VK_CHECK(vkQueuePresentKHR(graphics_queue_, &present_info));

  frame_++;*/