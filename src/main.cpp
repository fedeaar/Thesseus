#include "camera/camera.h"
#include "event/event.h"
#include "render/include.h"

//
// global
//
render::Engine::Params global{
  1280,
  720,
  "Thesseus",
};

//
// main
//

int
main(int argc, char* args[])
{
  Camera camera;
  render::Engine engine{ global, &camera };
  EventLoop main{ &engine, &camera };
  if (main.run() != core::code::SUCCESS) {
    return 1;
  }
  return 0;
}
