#include "../collection.h"

const Shader::input shaders::Default3d::format[1] = {{gl_float, 3}};

shaders::Default3d::Default3d() : Shader() {}

shaders::Default3d::~Default3d() {}

void shaders::Default3d::load() {
  Shader::load("./shader/collection/default-3d/main.vs",
               "./shader/collection/default-3d/main.fs");
}

void shaders::Default3d::set_model(const m4f& model) {
  set_uniform("model", model);
}

void shaders::Default3d::set_view(const m4f& view) {
  set_uniform("view", view);
}

void shaders::Default3d::set_projection(const m4f& proj) {
  set_uniform("projection", proj);
}

void shaders::Default3d::set_object_color(const v3f& color) {
  set_uniform("object_color", color);
}

void shaders::Default3d::set_light_color(const v3f& color) {
  set_uniform("light_color", color);
}
