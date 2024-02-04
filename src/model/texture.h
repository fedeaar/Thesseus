#ifndef MODEL_TEXTURE_
#define MODEL_TEXTURE_

#include "../core/io/io.h"
#include "../core/types.h"
#include <GL/glew.h>
#include <string>
#include <vector>

struct texture_param
{
  GLenum name;
  GLuint value;
};

class Texture2D {

private:
  i32 texture_width, texture_height, texture_channels;
  u8* image;
  GLuint texture;
  const std::string path;

public:
  Texture2D(const std::string& path);
  ~Texture2D();

  void destroy();
  void load(const std::vector<texture_param>& params);
  void bind(GLenum texture_number);
};

#endif // MODEL_TEXTURE_
