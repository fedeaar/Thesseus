#include "default-3d-textured-directional-light.h"

const Shader::input shaders::Default3dTexturedDirectional::format[2] = {
    {gl_float, 3}, {gl_float, 2}};

shaders::Default3dTexturedDirectional::Default3dTexturedDirectional()
    : shaders::Default3d() {}

shaders::Default3dTexturedDirectional::~Default3dTexturedDirectional() {}

void shaders::Default3dTexturedDirectional::load() {
  Shader::load("./shader/collection/default-3d-textured/main.vs",
               "./shader/collection/default-3d-textured/main.fs");
}

void shaders::Default3dTexturedDirectional::set_material_diffuse_map(
    u32 texture) {
  set_uniform("material.diffuse", texture);
}

void shaders::Default3dTexturedDirectional::set_material_specular_map(
    u32 texture) {
  set_uniform("material.specular", texture);
}

void shaders::Default3dTexturedDirectional::set_light_direction(v3f direction) {
  set_uniform("light.direction", direction);
}

void shaders::Default3dTexturedDirectional::set_light_attenuation_constant(
    f32 value) {
  set_uniform("light.constant", value);
}

void shaders::Default3dTexturedDirectional::set_light_attenuation_linear(
    f32 value) {
  set_uniform("light.linear", value);
}

void shaders::Default3dTexturedDirectional::set_light_attenuation_quadratic(
    f32 value) {
  set_uniform("light.quadratic", value);
}
