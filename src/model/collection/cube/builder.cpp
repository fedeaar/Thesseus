#include "../collection.h"
#include "raw.h"

Model<shaders::Default3d> models::cube::create_default3d_model() {
  std::shared_ptr<void> cube_ptr((void*)&data, [](void*) {});  // is static
  auto fmt_vec =
      std::vector<Raw::attribute>(std::begin(format), std::end(format));
  const Raw raw(cube_ptr, sizeof(data), fmt_vec);
  Reformatter rfmt(raw, {{Reformatter::MOVE, move_action : {0}}});
  auto raw_ptr = std::make_shared<Raw>(rfmt.get());
  auto vao_ptr = std::make_shared<VAO>(raw_ptr);
  return Model<shaders::Default3d>(vao_ptr);
}

Model<shaders::Default3dTextured>
models::cube::create_default3dTextured_model() {
  std::shared_ptr<void> cube_ptr((void*)&data, [](auto*) {});  // is static
  auto fmt_vec =
      std::vector<Raw::attribute>(std::begin(format), std::end(format));
  auto raw_ptr = std::make_shared<Raw>(cube_ptr, sizeof(data), fmt_vec);
  auto vao_ptr = std::make_shared<VAO>(raw_ptr);
  return Model<shaders::Default3dTextured>(vao_ptr);
}
