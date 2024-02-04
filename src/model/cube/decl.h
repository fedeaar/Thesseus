#ifndef MODEL_CUBE_BASIC_
#define MODEL_CUBE_BASIC_

#include "../../core/types.h"
#include "../../shader/default-3d/decl.h"
#include "../../shader/shader.h"
#include "../model.hpp"
#include "../vao.h"
#include <functional>
#include <vector>

namespace models {

namespace cube {

Model<shaders::Default3d> create();

}; // namespace cube

}; // namespace models

#endif // MODEL_CUBE_BASIC_
