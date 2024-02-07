#ifndef VAO_
#define VAO_

#include <GL/glew.h>

#include <memory>
#include <vector>

#include "../core/types.h"
#include "../raw/raw.h"
#include "../shader/shader.h"

class VAO {
 private:
  std::shared_ptr<Raw> raw_;
  GLuint vao_, vbo_;
  bool is_loaded_ = false;

 public:
  VAO(std::shared_ptr<Raw> raw_);
  ~VAO();

  void load();
  bool is_loaded();

  void destroy();

  void draw();  // TODO: Remove, should be in Model
};

#endif  // VAO_
