#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec3 fs_normal;
out vec3 fs_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

void main() {
  vec4 model_pos = model * vec4(pos, 1.0);
  gl_Position = projection * view * model_pos;
  fs_normal = normal_matrix * normal;
  fs_pos = vec3(model_pos);
}
