#ifndef TEXTURE_
#define TEXTURE_

#include <GL/glew.h>

#include <string>
#include <vector>

#include "../../core/io/io.h"
#include "../../core/types.h"

struct texture_param {
  GLenum name;
  GLuint value;
};

class Texture2D {
 private:
  i32 texture_width_, texture_height_, texture_channels_;
  u8* image_;
  GLuint texture_;
  const std::string path_;

 public:
  Texture2D(const std::string& path);
  ~Texture2D();

  void destroy();
  void load(const std::vector<texture_param>& params);
  void bind(GLenum texture_number);
};

#endif  // TEXTURE_
