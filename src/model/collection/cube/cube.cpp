#include "cube.h"

Model<shaders::Default3d> models::cube::create_default3d_model() {
  std::shared_ptr<void> cube_ptr((void*)&data, [](auto*) {});  // is static
  std::vector<Raw::attribute> fmt_vec = {
      {{gl_float, 3}, true},       // position
      {{gl_float, 3}, true},       // normals (skip)
      {{gl_float, 2}, true, true}  // texture coords (skip)
  };
  auto raw_ptr = std::make_shared<Raw>(cube_ptr, sizeof(data), fmt_vec);
  auto vao_ptr = std::make_shared<VAO>(raw_ptr);
  return Model<shaders::Default3d>(vao_ptr);
}

Model<shaders::Default3dTextured>
models::cube::create_default3dTextured_model() {
  std::shared_ptr<void> cube_ptr((void*)&data, [](auto*) {});  // is static
  std::vector<Raw::attribute> fmt_vec = {
      {{gl_float, 3}, true},  // position
      {{gl_float, 3}, true},  // normals
      {{gl_float, 2}, true}   // texture coords
  };
  auto raw_ptr = std::make_shared<Raw>(cube_ptr, sizeof(data), fmt_vec);
  auto vao_ptr = std::make_shared<VAO>(raw_ptr);
  return Model<shaders::Default3dTextured>(vao_ptr);
}

Model<shaders::LightSource> models::cube::create_lightSource_model() {
  std::shared_ptr<void> cube_ptr((void*)&data, [](auto*) {});  // is static
  std::vector<Raw::attribute> fmt_vec = {
      {{gl_float, 3}, true},        // position
      {{gl_float, 3}, true, true},  // normals (skip)
      {{gl_float, 2}, true, true}   // texture coords (skip)
  };
  auto raw_ptr = std::make_shared<Raw>(cube_ptr, sizeof(data), fmt_vec);
  auto vao_ptr = std::make_shared<VAO>(raw_ptr);
  return Model<shaders::LightSource>(vao_ptr);
}

Model<shaders::Default3dTexturedDirectional>
models::cube::create_default3dTexturedDirectional_model() {
  std::shared_ptr<void> cube_ptr((void*)&data, [](auto*) {});  // is static
  std::vector<Raw::attribute> fmt_vec = {
      {{gl_float, 3}, true},  // position
      {{gl_float, 3}, true},  // normals
      {{gl_float, 2}, true}   // texture coords
  };
  auto raw_ptr = std::make_shared<Raw>(cube_ptr, sizeof(data), fmt_vec);
  auto vao_ptr = std::make_shared<VAO>(raw_ptr);
  return Model<shaders::Default3dTexturedDirectional>(vao_ptr);
}
