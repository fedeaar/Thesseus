#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct Light {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  // attenuation
  float constant;
  float linear;
  float quadratic;
};


in vec3 fs_normal;
in vec3 fs_pos;
in vec2 fs_texture_coords;

out vec4 out_color;

uniform vec3 view_pos;
uniform Material material; 
uniform Light light;

void main() {
  vec3 tex = vec3(texture(material.diffuse, fs_texture_coords));
  // ambient
  float light_distance = length(light.position - FragPos);
  float attenuation = 1.0 / (light.constant + light.linear * light_distance + 
    light.quadratic * (light_distance * light_distance));
  vec3 ambient = light.ambient * tex;
  // diffusion
  vec3 norm = normalize(fs_normal);
  vec3 light_dir = normalize(-light.direction);
  float diff = max(dot(norm, light_dir), 0.0);
  vec3 diffusion = diff * light.diffuse * tex;
  // specular
  vec3 view_dir = normalize(view_pos - fs_pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
  vec3 specular =  spec * vec3(texture(material.specular, fs_texture_coords)) * light.specular;
  // result (phong lighting)
  out_color = vec4(attenuation * (ambient + diffusion + specular), 1.0);
}
