#version 330 core

in vec3 fs_normal;
in vec3 fs_pos;

out vec4 out_color;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_pos;

void main() {
  vec3 ambient = 0.1 * light_color;
  vec3 norm = normalize(fs_normal);
  vec3 light_dir = normalize(light_pos - fs_pos);
  float diff = max(dot(norm, light_dir), 0.0);
  vec3 diffusion = diff * light_color;
  out_color = vec4((ambient + diffusion) * object_color, 1.0);
}
