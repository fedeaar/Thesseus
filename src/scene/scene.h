#ifndef SCENE_
#define SCENE_

#include "../camera/camera.h"
#include "../model/collection/collection.h"
#include "../shader/collection/collection.h"

namespace scene {

void init();
void destroy();
void render(const Camera& pov);

}  // namespace scene

#endif  // SCENE_
