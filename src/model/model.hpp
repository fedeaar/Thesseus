#ifndef MODEL_MODEL_
#define MODEL_MODEL_

#include "../core/types.h"
#include "../shader/shader.h"
#include "texture.h"
#include "vao.h"
#include <glm/glm.hpp>
#include <memory>

template <typename T>
class Model {
  static_assert(
    std::is_base_of<shaders::Default3d, T>::value,
    "template must derive from shaders::Default3d");

private:
  std::shared_ptr<VAO> vao;
  v3f position, axis;
  f32 angle;

public:
  Model(
    std::shared_ptr<VAO> vao,
    v3f position = {0.0f, 0.0f, 0.0f},
    f32 angle = 0.0f,
    v3f axis = {0.0f, 0.0f, 1.0f});
  ~Model();

  void rotate(f32 new_angle, v3f new_axis);
  void place(v3f new_position);

  void render(T& shader);
};

#include "model.tpp"

#endif // MODEL_MODEL_
