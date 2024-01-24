#ifndef VAO_
#define VAO_

#include "core/types.h"
#include <GL/glew.h>
#include <vector>

struct attribute
{
  GLuint gl_type, length;
  GLboolean normalize;
};

template <typename T>
class VAO {

private:
  std::vector<T> buffer;
  std::vector<attribute> structure;
  GLuint vao, vbo;
  bool _is_loaded = false;

public:
  VAO(const std::vector<T>& buffer, const std::vector<attribute>& structure);
  ~VAO();

  void destroy();

  bool is_loaded();

  void load();
  void draw();
};

#include "vao.tpp"

#endif // VAO_
