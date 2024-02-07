#version 330 core

in vec2 texture_coords;
uniform sampler2d texture_2d;

out vec4 out_color;

void main() {
  out_color = texture(texture_2d, texture_coords);
}
