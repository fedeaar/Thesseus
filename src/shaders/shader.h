#ifndef SHADERS_
#define SHADERS_

#include "../core/io/io.h"
#include "../core/types.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader {
protected:
  GLuint handle = 0;
  bool bound = false;

  void attach(const std::string& vs_source, const std::string& fs_source);

public:
  enum type
  {
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
    VERTEX_SHADER = GL_VERTEX_SHADER
  };

  Shader();
  ~Shader();

  void use();
  void destroy();

  // TODO: make protected
  void load(const std::string& vs_path, const std::string& fs_path);

  // TODO: make protected
  void set_uniform(const std::string& ref, bool val);
  void set_uniform(const std::string& ref, int val);
  void set_uniform(const std::string& ref, float val);
  void set_uniform(const std::string& ref, const v3f& val);
  void set_uniform(const std::string& ref, const v4f& val);
  void set_uniform(const std::string& ref, const m4f& val);
};

#endif // SHADERS_
