#include "basic.h"

std::vector<models::cube::data> buffer = {
  {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f},
  { 0.5f, -0.5f, -0.5f, 1.0f, 0.0f},
  { 0.5f,  0.5f, -0.5f, 1.0f, 1.0f},
  { 0.5f,  0.5f, -0.5f, 1.0f, 1.0f},
  {-0.5f,  0.5f, -0.5f, 0.0f, 1.0f},
  {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f},

  {-0.5f, -0.5f,  0.5f, 0.0f, 0.0f},
  { 0.5f, -0.5f,  0.5f, 1.0f, 0.0f},
  { 0.5f,  0.5f,  0.5f, 1.0f, 1.0f},
  { 0.5f,  0.5f,  0.5f, 1.0f, 1.0f},
  {-0.5f,  0.5f,  0.5f, 0.0f, 1.0f},
  {-0.5f, -0.5f,  0.5f, 0.0f, 0.0f},

  {-0.5f,  0.5f,  0.5f, 1.0f, 0.0f},
  {-0.5f,  0.5f, -0.5f, 1.0f, 1.0f},
  {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
  {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
  {-0.5f, -0.5f,  0.5f, 0.0f, 0.0f},
  {-0.5f,  0.5f,  0.5f, 1.0f, 0.0f},

  { 0.5f,  0.5f,  0.5f, 1.0f, 0.0f},
  { 0.5f,  0.5f, -0.5f, 1.0f, 1.0f},
  { 0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
  { 0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
  { 0.5f, -0.5f,  0.5f, 0.0f, 0.0f},
  { 0.5f,  0.5f,  0.5f, 1.0f, 0.0f},

  {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
  { 0.5f, -0.5f, -0.5f, 1.0f, 1.0f},
  { 0.5f, -0.5f,  0.5f, 1.0f, 0.0f},
  { 0.5f, -0.5f,  0.5f, 1.0f, 0.0f},
  {-0.5f, -0.5f,  0.5f, 0.0f, 0.0f},
  {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},

  {-0.5f,  0.5f, -0.5f, 0.0f, 1.0f},
  { 0.5f,  0.5f, -0.5f, 1.0f, 1.0f},
  { 0.5f,  0.5f,  0.5f, 1.0f, 0.0f},
  { 0.5f,  0.5f,  0.5f, 1.0f, 0.0f},
  {-0.5f,  0.5f,  0.5f, 0.0f, 0.0f},
  {-0.5f,  0.5f, -0.5f, 0.0f, 1.0f}
};

std::vector<attribute> attr = {
  {GL_FLOAT, 3, GL_FALSE},
  {GL_FLOAT, 2, GL_FALSE},
};

VAO<models::cube::data> vao(buffer, attr);

Model<models::cube::data>
models::cube::create(v3f position, f32 angle, v3f axis) {
  std::shared_ptr<VAO<models::cube::data>> vao_ptr(
    &vao, [](VAO<models::cube::data>*) {}); // is static
  return Model<models::cube::data>(vao_ptr, position, angle, axis);
}
