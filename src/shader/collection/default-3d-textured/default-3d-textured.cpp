#include "default-3d-textured.h"

const Shader::input shaders::Default3dTextured::format[2] = {{gl_float, 3},
                                                             {gl_float, 2}};

shaders::Default3dTextured::Default3dTextured() : shaders::Default3d() {}

shaders::Default3dTextured::~Default3dTextured() {}

void shaders::Default3dTextured::load() {
  Shader::load("./shader/collection/default-3d-textured/main.vs",
               "./shader/collection/default-3d-textured/main.fs");
}
void shaders::Default3dTextured::set_texture(const u32& number) {
  set_uniform("texture_2d", number);
}
