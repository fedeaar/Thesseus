#ifndef MODEL_VAO_
#define MODEL_VAO_

#include <GL/glew.h>

#include <memory>
#include <vector>

#include "../core/types.h"
#include "../shader/shader.h"

class VAO {
 public:
  struct attribute {
    Shader::input format;
    GLboolean normalize;
  };
  static inline u32 stride(const std::vector<VAO::attribute>& structure) {
    u32 stride = 0;
    for (const VAO::attribute& data : structure) {
      stride += data.format.length * sizeof(data.format.gl_type);
    }
    return stride;
  }

 private:
  std::shared_ptr<void> raw_;
  u32 size_, length_, stride_;
  std::vector<attribute> structure_;
  GLuint vao_, vbo_;
  bool is_loaded_ = false;

 public:
  VAO(std::shared_ptr<void> raw, u32 size,
      const std::vector<attribute>& structure);
  ~VAO();

  void load();
  bool is_loaded();

  void destroy();

  void draw();
};

#endif  // MODEL_VAO_
