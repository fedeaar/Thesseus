#version 330 core

uniform vec3 object_color;
uniform vec3 light_color;

out vec4 out_color;

void main() {
  float ambientStrength = 0.1;
  vec3 ambient = ambientStrength * light_color;

  out_color = vec4(ambient * object_color, 1.0);
}
