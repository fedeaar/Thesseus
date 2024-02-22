#ifndef SHADER_COLLECTION_
#define SHADER_COLLECTION_

#include <glm/gtc/type_ptr.hpp>

#include "../../core/types.h"
#include "../shader.h"

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
  void set_object_color(const v3f& color);
  void set_light_color(const v3f& color);
  void set_light_pos(const v3f& pos);
  void set_view_pos(const v3f& pos);
};

class Default3dTextured : public Default3d {
 public:
  static const Shader::input format[2];

  Default3dTextured();
  ~Default3dTextured();

  void load();

  void set_texture(const u32& number);
};

class LightSource : public Default3d {
 public:
  static const Shader::input format[1];

  LightSource();
  ~LightSource();

  void load();
};

}  // namespace shaders

#endif  // SHADER_COLLECTION_
