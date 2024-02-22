#version 330 core

in vec2 fs_texture_coords;

out vec4 out_color;

uniform sampler2D texture_2d;

void main() {
  out_color = texture(texture_2d, fs_texture_coords);
}
