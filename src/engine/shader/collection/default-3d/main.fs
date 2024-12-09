#version 330 core

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec3 fs_normal;
in vec3 fs_pos;

out vec4 out_color;

uniform vec3 view_pos;
uniform Material material; 
uniform Light light;

void main() {
  // ambient
  vec3 ambient = (material.ambient) * light.ambient;
  // diffusion
  vec3 norm = normalize(fs_normal);
  vec3 light_dir = normalize(light.position - fs_pos);
  float diff = max(dot(norm, light_dir), 0.0);
  vec3 diffusion = (diff * material.diffuse) * light.diffuse;
  // specular
  vec3 view_dir = normalize(view_pos - fs_pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
  vec3 specular =  (spec * material.specular) * light.specular;
  // result (phong lighting)
  out_color = vec4(ambient + diffusion + specular, 1.0);
}
