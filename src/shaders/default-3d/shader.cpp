#include "shader.h"

shaders::Default3d::Default3d() : Shader() {
  load("./shaders/default-3d/main.vs", "./shaders/default-3d/main.fs");
}

shaders::Default3d::~Default3d() {}

void shaders::Default3d::set_model(const m4f& model) {
  // TODO: error handling
  glUniformMatrix4fv(
    glGetUniformLocation(handle, "model"), 1, GL_FALSE, glm::value_ptr(model));
}

void shaders::Default3d::set_view(const m4f& view) {
  // TODO: error handling
  glUniformMatrix4fv(
    glGetUniformLocation(handle, "view"), 1, GL_FALSE, glm::value_ptr(view));
}

void shaders::Default3d::set_projection(const m4f& proj) {
  // TODO: error handling
  glUniformMatrix4fv(
    glGetUniformLocation(handle, "projection"),
    1,
    GL_FALSE,
    glm::value_ptr(proj));
}

void shaders::Default3d::set_object_color(const v3f& color) {
  // TODO: error handling
  glUniform3f(
    glGetUniformLocation(handle, "object_color"), color.x, color.y, color.z);
}

void shaders::Default3d::set_light_color(const v3f& color) {
  // TODO: error handling
  glUniform3f(
    glGetUniformLocation(handle, "light_color"), color.x, color.y, color.z);
}
