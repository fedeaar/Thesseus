#include "../scene.h"

//
// global
//

shaders::Default3dTextured program;
shaders::LightSource lightProgram;
Model<shaders::Default3dTextured> cube =
    models::cube::create_default3dTextured_model();
Model<shaders::LightSource> light = models::cube::create_lightSource_model();
Texture2D texture("./res/container2.png");
Texture2D specular_map("./res/container2_specular.png");
std::vector<texture_param> params = {{GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT},
                                     {GL_TEXTURE_WRAP_T, GL_REPEAT},
                                     {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
                                     {GL_TEXTURE_MAG_FILTER, GL_LINEAR}};
v3f light_pos = {1.2f, 1.0f, 2.0f};

//
// scene
//

i32 Scene::init() {
  program.load();
  lightProgram.load();
  texture.load(params);
  light.scale({v3f(0.2f)});
  light.place(light_pos);
  program.use();
  program.set_material_diffuse_map(0);
  program.set_material_specular_map(1);
  program.set_material_specular({0.5f, 0.5f, 0.5f});
  program.set_material_shininess(32.0f);
  program.set_light_ambient({0.2f, 0.2f, 0.2f});
  program.set_light_diffuse({0.5f, 0.5f, 0.5f});
  program.set_light_specular({1.0f, 1.0f, 1.0f});
  program.set_light_position(light_pos);
  lightProgram.use();
  lightProgram.set_light_color(v3f{1.0});
  return 1;
}

i32 Scene::destroy() {
  texture.destroy();
  program.destroy();
  lightProgram.destroy();
  return 1;
}

void Scene::render(const Camera& camera) {
  texture.bind(GL_TEXTURE0);
  specular_map.bind(GL_TEXTURE1);
  lightProgram.use();
  lightProgram.set_view(camera.view_matrix());
  lightProgram.set_projection(camera.proj_matrix());
  light.render(lightProgram);
  program.use();
  program.set_view(camera.view_matrix());
  program.set_view_pos(camera.position());
  program.set_projection(camera.proj_matrix());
  cube.render(program);
}
