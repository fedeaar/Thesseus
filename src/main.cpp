#include "camera/camera.h"
#include "engine/engine.h"
#include "event/event.h"
#include "scene/scene.h"

//
// global
//

RenderParams global{
    640,        // screen_width
    480,        // screen_height
    "Thesseus"  // name
};

//
// main
//

int main(int argc, char* args[]) {
  RenderEngine engine{global};
  Camera camera(
      engine.get_aspect_ratio());  // FIXME: do not set aspect ratio this way
  Scene scene;
  EventLoop main{&engine, &camera, &scene};
  return main.run();
}
