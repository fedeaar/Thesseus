#ifndef SHADER_COLLECTION_DEFAULT3D_
#define SHADER_COLLECTION_DEFAULT3D_

#include <glm/gtc/type_ptr.hpp>

#include "../../../core/types.h"
#include "../../shader.h"

namespace shaders {

class Default3d : public Shader {
 public:
  static const Shader::input format[2];

  Default3d();
  ~Default3d();

  void load();

  void set_model(const m4f& model);
  void set_view(const m4f& view);
  void set_projection(const m4f& proj);
  void set_normal_matrix(const m3f& normal);
  void set_view_pos(const v3f& pos);
  void set_material_ambient(const v3f& ambient);
  void set_material_diffuse(const v3f& diffuse);
  void set_material_specular(const v3f& specular);
  void set_material_shininess(f32 shininess);
  void set_light_ambient(const v3f& ambient);
  void set_light_diffuse(const v3f& diffuse);
  void set_light_specular(const v3f& specular);
  void set_light_position(const v3f& pos);
};

}  // namespace shaders

#endif  // SHADER_COLLECTION_DEFAULT3D_
