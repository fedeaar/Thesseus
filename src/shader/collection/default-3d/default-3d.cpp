#include "default-3d.h"

const Shader::input shaders::Default3d::format[2] = {{gl_float, 3},
                                                     {gl_float, 3}};

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

void shaders::Default3d::set_normal_matrix(const m3f& normal) {
  set_uniform("normal_matrix", normal);
}

void shaders::Default3d::set_light_color(const v3f& color) {
  set_uniform("light_color", color);
}

void shaders::Default3d::set_light_pos(const v3f& pos) {
  set_uniform("light_pos", pos);
}

void shaders::Default3d::set_view_pos(const v3f& pos) {
  set_uniform("view_pos", pos);
}

void shaders::Default3d::set_material_ambient(const v3f& ambient) {
  set_uniform("material.ambient", ambient);
}

void shaders::Default3d::set_material_diffuse(const v3f& diffuse) {
  set_uniform("material.diffuse", diffuse);
}

void shaders::Default3d::set_material_specular(const v3f& specular) {
  set_uniform("material.specular", specular);
}

void shaders::Default3d::set_material_shininess(f32 shininess) {
  set_uniform("material.shininess", shininess);
}
