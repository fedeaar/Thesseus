#include "texture.h"

Texture2D::Texture2D(const std::string& path) : path(path) {}

Texture2D::~Texture2D() { destroy(); }

void Texture2D::destroy() { glDeleteTextures(1, &texture); }

void Texture2D::load(const std::vector<texture_param>& params) {
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  for (const texture_param& param : params) {
    glTexParameteri(GL_TEXTURE_2D, param.name, param.value);
  }
  image = io::image::raw(path, &texture_width, &texture_height,
                         &texture_channels, 0);
  // TODO: all formats
  GLuint format;
  if (texture_channels == 3) {
    format = GL_RGB;
  } else if (texture_channels == 4) {
    format = GL_RGBA;
  }
  if (image) {
    glTexImage2D(GL_TEXTURE_2D, 0, format, texture_width, texture_height, 0,
                 format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    // TODO: proper error handling
    std::cerr << "failed to load image." << std::endl;
  }
  io::image::free(image);
}

void Texture2D::bind(GLenum texture_number) {
  glActiveTexture(texture_number);
  glBindTexture(GL_TEXTURE_2D, texture);
}
