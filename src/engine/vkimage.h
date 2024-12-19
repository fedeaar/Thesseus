#ifndef VKIMG_
#define VKIMG_

#include "types.h"
#include "vkinit.h"

namespace vkutil {

void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current,
                      VkImageLayout next);
}

#endif  // VKIMG_
