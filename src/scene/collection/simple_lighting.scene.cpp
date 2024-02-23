#include "../scene.h"
#include "glm/trigonometric.hpp"
//
// global
//

shaders::Default3d program;
shaders::LightSource lightProgram;
Model<shaders::Default3d> cube = models::cube::create_default3d_model();
Model<shaders::LightSource> light = models::cube::create_lightSource_model();
f32 dist = 3;
f32 angle = 0;
v3f light_pos;

//
// scene
//

void scene::init() {
  program.load();
  lightProgram.load();
  light.scale({v3f(0.2f)});
  cube.scale(v3f(3));
  program.use();
  program.set_object_color({1.0, 0.5, 0.31});
  program.set_light_color({1.0, 1.0, 1.0});
}

void scene::destroy() { program.destroy(); }

f32 i = 0;
void scene::render(const Camera& camera) {
  angle = glm::radians(i++);
  light_pos = {cos(angle) * dist, sin(angle) * dist, 0.0};
  lightProgram.use();
  lightProgram.set_view(camera.view_matrix());
  lightProgram.set_projection(camera.proj_matrix());
  light.place(light_pos);
  light.render(lightProgram);
  program.use();
  program.set_view(camera.view_matrix());
  program.set_view_pos(camera.position());
  program.set_projection(camera.proj_matrix());
  program.set_light_pos(light_pos);
  cube.render(program);
}
