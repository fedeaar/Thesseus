#include "../scene.h"

//
// global
//

shaders::Default3d program1;
shaders::Default3dTextured program2;
Model<shaders::Default3d> cube1 = models::cube::create_default3d_model();
Model<shaders::Default3dTextured> cube2 =
    models::cube::create_default3dTextured_model();
Texture2D texture("./res/container.jpg");
std::vector<texture_param> params = {{GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT},
                                     {GL_TEXTURE_WRAP_T, GL_REPEAT},
                                     {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
                                     {GL_TEXTURE_MAG_FILTER, GL_LINEAR}};

//
// scene
//

void scene::init() {
  program1.load();
  program2.load();
  texture.load(params);
  program2.set_texture(0);
  cube2.place({5, 2, 0});
}

void scene::destroy() {
  texture.destroy();
  program1.destroy();
  program2.destroy();
}

void scene::render(const Camera& camera) {
  texture.bind(GL_TEXTURE0);
  program1.use();
  program1.set_view(camera.view_matrix());
  program1.set_projection(camera.proj_matrix());
  cube1.render(program1);
  program2.use();
  program2.set_view(camera.view_matrix());
  program2.set_projection(camera.proj_matrix());
  cube2.render(program2);
}
