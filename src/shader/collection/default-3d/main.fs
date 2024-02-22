#version 330 core

in vec3 fs_normal;
in vec3 fs_pos;

out vec4 out_color;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 view_pos;

void main() {
  // ambient
  float ambient_strength = 0.1;
  vec3 ambient = ambient_strength * light_color;
  // diffusion
  vec3 norm = normalize(fs_normal);
  vec3 light_dir = normalize(light_pos - fs_pos);
  float diff = max(dot(norm, light_dir), 0.0);
  vec3 diffusion = diff * light_color;
  // specular
  float specular_strength = 0.5;
  vec3 view_dir = normalize(view_pos - fs_pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
  vec3 specular = specular_strength * spec * light_color;
  // result (phong lighting)
  out_color = vec4((ambient + diffusion + specular) * object_color, 1.0);
}
