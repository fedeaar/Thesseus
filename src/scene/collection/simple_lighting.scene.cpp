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
v3f light_color;

//
// scene
//

i32 Scene::init() {
  program.load();
  lightProgram.load();
  light.scale({v3f(0.2f)});
  cube.scale(v3f(3));
  program.use();
  program.set_material_ambient({1.0f, 0.5f, 0.31f});
  program.set_material_diffuse({1.0f, 0.5f, 0.31f});
  program.set_material_specular({0.5f, 0.5f, 0.5f});
  program.set_material_shininess(32.0f);
  program.set_light_ambient({0.2f, 0.2f, 0.2f});
  program.set_light_diffuse({0.5f, 0.5f, 0.5f});
  program.set_light_specular({1.0f, 1.0f, 1.0f});
  lightProgram.use();
  lightProgram.set_light_color(v3f{1.0});
  return 1;
}

i32 Scene::destroy() {
  program.destroy();
  return 1;
}

f32 i = 0;
void Scene::render(const Camera& camera) {
  angle = glm::radians(i++);
  light_pos = {cos(angle) * dist, sin(angle) * dist, 0.0};
  // light_color.x = sin(angle * 2.0f) + 0.1;
  // light_color.y = sin(angle * 0.7f) + 0.1;
  // light_color.z = sin(angle * 1.3f) + 0.1;
  // program.set_light_ambient(light_color * v3f(0.5));
  // program.set_light_diffuse(light_color * v3f(0.5 * 0.2));
  lightProgram.use();
  lightProgram.set_view(camera.view_matrix());
  lightProgram.set_projection(camera.proj_matrix());
  // lightProgram.set_light_color(light_color);
  light.place(light_pos);
  light.render(lightProgram);
  program.use();
  program.set_view(camera.view_matrix());
  program.set_view_pos(camera.position());
  program.set_projection(camera.proj_matrix());
  program.set_light_position(light_pos);
  cube.render(program);
}
