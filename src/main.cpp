#include "camera/camera.h"
#include "event/event.h"
#include "render/include.h"

//
// global
//

render::Engine::Params global{
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
  render::Engine engine{ global };
  Camera camera{ engine.state.window_mgr.state.aspect_ratio };
  EventLoop main{ &engine, &camera };
  if (main.run() != core::code::SUCCESS) {
    return 1;
  }
  return 0;
}
