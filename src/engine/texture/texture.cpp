#include "texture.h"

Texture2D::Texture2D(const std::string& path) : path_(path) {}

Texture2D::~Texture2D() { destroy(); }

void Texture2D::destroy() { glDeleteTextures(1, &texture_); }

void Texture2D::load(const std::vector<texture_param>& params) {
  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);
  for (const texture_param& param : params) {
    glTexParameteri(GL_TEXTURE_2D, param.name, param.value);
  }
  image_ = io::image::raw(path_, &texture_width_, &texture_height_,
                         &texture_channels_, 0);
  // TODO: all formats
  GLuint format;
  if (texture_channels_ == 3) {
    format = GL_RGB;
  } else if (texture_channels_ == 4) {
    format = GL_RGBA;
  }
  if (image_) {
    glTexImage2D(GL_TEXTURE_2D, 0, format, texture_width_, texture_height_, 0,
                 format, GL_UNSIGNED_BYTE, image_);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    // TODO: proper error handling
    std::cerr << "failed to load image." << std::endl;
  }
  io::image::free(image_);
}

void Texture2D::bind(GLenum texture_number) {
  glActiveTexture(texture_number);
  glBindTexture(GL_TEXTURE_2D, texture_);
}
