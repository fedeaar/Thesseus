#ifndef MODEL_COLLECTION_
#define MODEL_COLLECTION_

#include <functional>
#include <vector>

#include "../../core/types.h"
#include "../../raw/reformatter/reformatter.h"
#include "../../shader/collection/collection.h"
#include "../../shader/shader.h"
#include "../../vao/vao.h"
#include "../model.hpp"

namespace models {

namespace cube {

Model<shaders::Default3d> create_default3d_model();
Model<shaders::Default3dTextured> create_default3dTextured_model();
Model<shaders::LightSource> create_lightSource_model();

};  // namespace cube

};  // namespace models

#endif  // MODEL_COLLECTION_
