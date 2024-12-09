#ifndef SHADER_COLLECTION_DEFAULT3DTEXTURED_
#define SHADER_COLLECTION_DEFAULT3DTEXTURED_

#include <glm/gtc/type_ptr.hpp>

#include "../../../../core/types.h"
#include "../../shader.h"
#include "../default-3d/default-3d.h"

namespace shaders {

class Default3dTextured : public Default3d {
 public:
  static const Shader::input format[2];

  Default3dTextured();
  ~Default3dTextured();

  void load();

  void set_material_diffuse_map(u32 texture);
  void set_material_specular_map(u32 texture);
};

}  // namespace shaders

#endif  // SHADER_COLLECTION_DEFAULT3DTEXTURED_
