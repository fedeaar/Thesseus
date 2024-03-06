#ifndef SHADER_COLLECTION_LIGHTSOURCE_
#define SHADER_COLLECTION_LIGHTSOURCE_

#include <glm/gtc/type_ptr.hpp>

#include "../../../core/types.h"
#include "../../shader.h"
#include "../default-3d/default-3d.h"

namespace shaders {

class LightSource : public Default3d {
 public:
  static const Shader::input format[1];

  LightSource();
  ~LightSource();

  void load();

  void set_light_color(const v3f& color);
};
}  // namespace shaders

#endif  // SHADER_COLLECTION_LIGHTSOURCE_
