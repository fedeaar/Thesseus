#version 330 core

in vec2 fs_tex_coord;
uniform float mix_intensity = 0.5;
uniform sampler2D texture1;
uniform sampler2D texture2;
out vec4 out_color;

void main() {
  out_color = mix(
    texture(texture1, fs_tex_coord),
    texture(texture2, fs_tex_coord),
    mix_intensity
  );
}
