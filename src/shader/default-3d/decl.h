#ifndef SHADER_DEFAULT3D_
#define SHADER_DEFAULT3D_

#include "../../core/types.h"
#include "../shader.h"
#include <glm/gtc/type_ptr.hpp>

namespace shaders {

class Default3d : public Shader {

public:
  static const Shader::input format[1];

  Default3d();
  ~Default3d();

  void load();

  void set_model(const m4f& model);
  void set_view(const m4f& view);
  void set_projection(const m4f& proj);
  void set_object_color(const v3f& color);
  void set_light_color(const v3f& color);
};
} // namespace shaders

#endif // SHADER_DEFAULT3D_
