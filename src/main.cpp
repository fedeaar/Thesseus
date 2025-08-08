#include "camera/camera.h"
#include "debug/debug.h"
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
debug::GlobalStats stats;

//
// main
//

int
main(int argc, char* args[])
{
  Camera camera;
  render::Engine engine{ global, &camera, &stats };
  EventLoop main{ &engine, &camera, &stats };
  if (main.run() != core::code::SUCCESS) {
    return 1;
  }
  return 0;
}
