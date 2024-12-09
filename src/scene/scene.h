#ifndef SCENE_
#define SCENE_

#include "../camera/camera.h"
#include "../engine/shader/collection/collection.h"
#include "../model/collection/collection.h"

class Scene {
 public:
  i32 init();
  i32 destroy();

  void render(const Camera& camera);
};

#endif  // SCENE_
