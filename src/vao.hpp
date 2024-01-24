#ifndef VAO_
#define VAO_

#include "core/types.h"
#include <GL/glew.h>
#include <vector>

struct attribute {
  GLuint gl_type, length;
  GLboolean normalize;
};

template <typename T>
class VAO {

private:
  std::vector<T> buffer;
  std::vector<attribute> structure;
  GLuint vao, vbo;

public:
  VAO(const std::vector<T>& buffer, const std::vector<attribute>& structure);
  ~VAO();

  void destroy();

  void load();
  void draw();
};

#include "vao.tpp"

#endif // VAO_
