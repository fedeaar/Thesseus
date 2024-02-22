#ifndef MODEL_
#define MODEL_

#include <glm/glm.hpp>
#include <memory>

#include "../core/types.h"
#include "../shader/shader.h"
#include "../texture/texture.h"
#include "../vao/vao.h"

template <typename T>
class Model {
  static_assert(std::is_base_of<shaders::Default3d, T>::value,
                "template must derive from shaders::Default3d");

 private:
  std::shared_ptr<VAO> vao_;
  std::shared_ptr<Texture2D> textures_[16] = {};
  v3f position_ = {0.0f, 0.0f, 0.0f};
  v3f scale_ = {1.0f, 1.0f, 1.0f};
  v3f axis_ = {0.0f, 0.0f, 1.0f};
  f32 angle_ = 0.0f;

 public:
  Model(std::shared_ptr<VAO> vao);
  ~Model();

  void use_texture(const std::shared_ptr<Texture2D>& texture, u8 position);

  void rotate(f32 angle, v3f axis);
  void place(v3f position);
  void scale(v3f scale);

  void render(T& shader);
};

#include "model.tpp"

#endif  // MODEL_
