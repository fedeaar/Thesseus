#include "event/event.h"
#include "render/engine/engine.h"

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
  EventLoop main{ &engine };
  return main.run();
}
