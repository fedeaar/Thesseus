#ifndef SHADER_COLLECTION_DEFAULT3DTEXTUREDDIRECTIONAL_
#define SHADER_COLLECTION_DEFAULT3DTEXTUREDDIRECTIONAL_

#include <glm/gtc/type_ptr.hpp>

#include "../../../core/types.h"
#include "../../shader.h"
#include "../default-3d/default-3d.h"

namespace shaders {

class Default3dTexturedDirectional : public Default3d {
 public:
  static const Shader::input format[2];

  Default3dTexturedDirectional();
  ~Default3dTexturedDirectional();

  void load();

  void set_material_diffuse_map(u32 texture);
  void set_material_specular_map(u32 texture);
  void set_light_direction(v3f direction);
  void set_light_attenuation_constant(f32 value);
  void set_light_attenuation_linear(f32 value);
  void set_light_attenuation_quadratic(f32 value);
};

}  // namespace shaders

#endif  // SHADER_COLLECTION_DEFAULT3DTEXTUREDDIRECTIONAL_
