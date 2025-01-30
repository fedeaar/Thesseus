#include "camera/camera.h"
#include "event/event.h"
#include "render-engine/engine/engine.h"

//
// global
//

RenderEngine::Engine::Params global{
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
  RenderEngine::Engine engine{ global };
  Camera camera(
    engine.get_aspect_ratio()); // FIXME: do not set aspect ratio this way
  EventLoop main{ &engine, &camera };
  return main.run();
}
