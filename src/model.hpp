#ifndef MODEL_
#define MODEL_

#include "core/types.h"
#include "shader.h"
#include "texture.h"
#include "vao.hpp"
#include <glm/glm.hpp>
#include <memory>

template <typename T>
class Model {
private:
  std::shared_ptr<VAO<T>> vao;
  v3f position, axis;
  f32 angle;

public:
  Model(std::shared_ptr<VAO<T>> vao, v3f position, f32 angle, v3f axis);
  ~Model();

  void rotate(f32 new_angle, v3f new_axis);
  void place(v3f new_position);

  void render(Shader& program);
};

#include "model.tpp"

#endif // MODEL_
