#ifndef MODEL_CUBE_BASIC_
#define MODEL_CUBE_BASIC_

#include <functional>
#include <vector>

#include "../../core/types.h"
#include "../../shader/collection/collection.h"
#include "../../shader/shader.h"
#include "../model.hpp"
#include "../vao.h"

namespace models {

namespace cube {

Model<shaders::Default3d> create();

};  // namespace cube

};  // namespace models

#endif  // MODEL_CUBE_BASIC_
