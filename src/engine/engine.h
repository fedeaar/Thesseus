#ifndef ENGINE_
#define ENGINE_

#include "../camera/camera.h"
#include "../core/types.h"
// #include "../scene/scene.h"
#include "types.h"
#include "vkimage.h"
#include "vkinit.h"

struct DeletionQueue {
  std::deque<std::function<void()>> deletors;
  void push_function(std::function<void()>&& function) {
    deletors.push_back(function);
  }
  void flush() {
    // reverse iterate the deletion queue to execute all the functions
    for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
      (*it)();  // call functors
    }
    deletors.clear();
  }
};

struct RenderParams {
  u32 screen_width, screen_height;
  string name;
};

struct FrameData {
  VkCommandPool command_pool;
  VkCommandBuffer main_command_buffer;
  VkSemaphore swapchain_semaphore, render_semaphore;
  VkFence render_fence;
  DeletionQueue del_queue;
};

constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanRenderEngine {
  bool initialized_ = false;
  u64 frame_ = 0;
  RenderParams params_;
  f32 aspect_ratio_;
  struct SDL_Window* window_ = nullptr;
  VkExtent2D extent_;
  VkInstance instance_;
  VkDebugUtilsMessengerEXT debug_messenger_;
  VkPhysicalDevice gpu_;
  VkDevice device_;
  VkSurfaceKHR surface_;
  VkSwapchainKHR swapchain_;
  VkFormat swapchain_image_fmt_;
  std::vector<VkImage> swapchain_img_;
  std::vector<VkImageView> swapchain_img_views_;
  VkExtent2D swapchain_extent_;
  DeletionQueue main_del_queue_;
  VmaAllocator allocator_;

 public:
  FrameData frames_[FRAME_OVERLAP];
  FrameData& get_current_frame();
  VkQueue graphics_queue_;
  u32 graphics_queue_family_;

 private:
  i32 init_vulkan();
  i32 init_swapchain();
  i32 init_commands();
  i32 init_sync_structures();

  i32 create_swapchain(u32 width, u32 height);
  i32 destroy_swapchain();

 public:
  VulkanRenderEngine(RenderParams& params);

  i32 init();
  i32 destroy();

  void render(const Camera& camera /*, Scene& scene*/);

  f32 get_aspect_ratio();
};

#endif  // ENGINE_
