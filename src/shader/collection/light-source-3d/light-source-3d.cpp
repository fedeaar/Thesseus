#include "light-source-3d.h"

const Shader::input shaders::LightSource::format[1] = {{gl_float, 3}};

shaders::LightSource::LightSource() : Default3d() {}

shaders::LightSource::~LightSource() {}

void shaders::LightSource::load() {
  Shader::load("./shader/collection/light-source-3d/main.vs",
               "./shader/collection/light-source-3d/main.fs");
}
