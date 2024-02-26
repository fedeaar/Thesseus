#version 330 core

struct material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
}

in vec3 fs_normal;
in vec3 fs_pos;

out vec4 out_color;

uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 view_pos;
uniform material material; 

void main() {
  // ambient
  vec3 ambient = material.ambient * light_color;
  // diffusion
  vec3 norm = normalize(fs_normal);
  vec3 light_dir = normalize(light_pos - fs_pos);
  float diff = max(dot(norm, light_dir), 0.0);
  vec3 diffusion = (diff * material.diffuse) * light_color;
  // specular
  vec3 view_dir = normalize(view_pos - fs_pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
  vec3 specular =  (spec * material.specular) * light_color;
  // result (phong lighting)
  out_color = vec4(ambient + diffusion + specular, 1.0);
}
