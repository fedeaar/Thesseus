#ifndef SHADER_
#define SHADER_

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

#include "../core/io/io.h"
#include "../core/types.h"

class Shader {
 private:
  GLuint handle = 0;
  bool bound = false;

 protected:
  void attach(const std::string& vs_source, const std::string& fs_source);
  void load(const std::string& vs_path, const std::string& fs_path);

  inline void set_uniform(const std::string& ref, bool val);
  inline void set_uniform(const std::string& ref, u32 val);
  inline void set_uniform(const std::string& ref, i32 val);
  inline void set_uniform(const std::string& ref, f32 val);
  inline void set_uniform(const std::string& ref, const v3f& val);
  inline void set_uniform(const std::string& ref, const v4f& val);
  inline void set_uniform(const std::string& ref, const m4f& val);

 public:
  enum shader_type {
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
    VERTEX_SHADER = GL_VERTEX_SHADER
  };

  struct input {
    GL_Type gl_type;
    u8 length;

    bool operator==(const input& rhs) const;
  };

  Shader();
  ~Shader();

  void use();
  void destroy();
};

#include "shader.inl"

#endif  // SHADER_
