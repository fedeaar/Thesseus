#include "../scene.h"

//
// global
//

shaders::Default3dTexturedDirectional program;
Model<shaders::Default3dTexturedDirectional> cube =
    models::cube::create_default3dTexturedDirectional_model();
shaders::LightSource light_program;
Model<shaders::LightSource> light = models::cube::create_lightSource_model();
Texture2D texture("./res/container2.png");
Texture2D specular_map("./res/container2_specular.png");
std::vector<texture_param> params = {{GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT},
                                     {GL_TEXTURE_WRAP_T, GL_REPEAT},
                                     {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
                                     {GL_TEXTURE_MAG_FILTER, GL_LINEAR}};

v3f cube_positions[10] = {{0.0f, 0.0f, 0.0f},    {2.0f, 5.0f, -15.0f},
                          {-1.5f, -2.2f, -2.5f}, {-3.8f, -2.0f, -12.3f},
                          {2.4f, -0.4f, -3.5f},  {-1.7f, 3.0f, -7.5f},
                          {1.3f, -2.0f, -2.5f},  {1.5f, 2.0f, -2.5f},
                          {1.5f, 0.2f, -1.5f},   {-1.3f, 1.0f, -1.5f}};

v3f light_pos = {3, 3, 3};
//
// scene
//

i32 Scene::init() {
  program.load();
  light_program.load();
  texture.load(params);
  program.use();
  program.set_material_diffuse_map(0);
  program.set_material_specular_map(1);
  program.set_material_specular({0.5f, 0.5f, 0.5f});
  program.set_material_shininess(32.0f);
  program.set_light_ambient({0.2f, 0.2f, 0.2f});
  program.set_light_diffuse({0.5f, 0.5f, 0.5f});
  program.set_light_specular({1.0f, 1.0f, 1.0f});
  program.set_light_direction(light_pos);
  program.set_light_attenuation_constant(1.0f);
  program.set_light_attenuation_linear(0.09f);
  program.set_light_attenuation_quadratic(0.032f);
  light_program.use();
  light_program.set_light_color(v3f{1.0});
  light.scale({v3f(0.2f)});
  light.place(light_pos);
  return 1;
}

i32 Scene::destroy() {
  texture.destroy();
  program.destroy();
  return 1;
}

void Scene::render(const Camera& camera) {
  texture.bind(GL_TEXTURE0);
  specular_map.bind(GL_TEXTURE1);
  program.use();
  program.set_view(camera.view_matrix());
  program.set_view_pos(camera.position());
  program.set_projection(camera.proj_matrix());
  for (u32 i = 0; i < 10; ++i) {
    cube.place(cube_positions[i]);
    float angle = 20.0f * i;
    cube.rotate(angle, {1.0f, 0.3f, 0.5f});
    cube.render(program);
  }
  light_program.use();
  light_program.set_view(camera.view_matrix());
  light_program.set_projection(camera.proj_matrix());

  light.render(light_program);
}
