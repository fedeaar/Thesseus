#ifndef MODEL_BASIC_
#define MODEL_BASIC_

#include "../core/types.h"
#include "../model.hpp"
#include "../vao.hpp"
#include <vector>

namespace models {

namespace cube {

struct __attribute__((__packed__)) data
{
  GLfloat x, y, z, s, t;
};

typedef Model<data> type;

Model<data> create(v3f position = {0.0f, 0.0f, 0.0f},
                   f32 angle = 0.0f,
                   v3f axis = {0.0f, 0.0f, 1.0f});

} // namespace cube

} // namespace models

#endif // MODEL_BASIC_
