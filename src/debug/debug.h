#pragma once

#include "../core/include.h"

namespace debug {

struct GlobalStats
{
  f32 frametime;
  u32 triangleCount;
  u32 drawcallCount;
  f32 meshDrawTime;
};

} // debug
