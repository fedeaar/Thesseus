#include "camera/camera.h"
#include "engine/engine.h"
#include "event/event.h"

//
// global
//

EngineParams global{
  1280,      // screen_width
  720,       // screen_height
  "Thesseus" // name
};

//
// main
//

int
main(int argc, char* args[])
{
  Engine engine{ global };
  Camera camera(
    engine.get_aspect_ratio()); // FIXME: do not set aspect ratio this way
  EventLoop main{ &engine, &camera };
  return main.run();
}
