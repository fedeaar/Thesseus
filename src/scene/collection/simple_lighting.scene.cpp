#include "../scene.h"

//
// global
//

shaders::Default3d program;
shaders::LightSource lightProgram;
Model<shaders::Default3d> cube = models::cube::create_default3d_model();
Model<shaders::LightSource> light = models::cube::create_lightSource_model();
v3f light_pos = {1.2, 1.0, 2.0};

//
// scene
//

void scene::init() {
  program.load();
  lightProgram.load();
  light.place(light_pos);
  light.scale({v3f(0.2f)});
  program.use();
  program.set_object_color({1.0, 0.5, 0.31});
  program.set_light_color({1.0, 1.0, 1.0});
  program.set_light_pos(light_pos);
}

void scene::destroy() { program.destroy(); }

void scene::render(const Camera& camera) {
  lightProgram.use();
  lightProgram.set_view(camera.view_matrix());
  lightProgram.set_projection(camera.proj_matrix());
  light.render(lightProgram);
  program.use();
  program.set_view(camera.view_matrix());
  program.set_projection(camera.proj_matrix());
  cube.render(program);
}