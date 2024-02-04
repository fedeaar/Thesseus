#include "shader.h"

//
// private
//

GLuint _compile(const std::string& source, Shader::shader_type type) {
  GLuint handle = glCreateShader(type);
  const char* c_source = source.c_str();
  glShaderSource(handle, 1, &c_source, NULL);
  glCompileShader(handle);
  GLint success;
  glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar buf[512];
    glGetShaderInfoLog(handle, (GLuint)512, NULL, buf);
    std::cerr << "ShaderCompileError: " + std::string(buf)
              << std::endl;  // TODO: proper exception
  }
  return handle;
}

GLuint _attach(const std::string& vs_source, const std::string& fs_source) {
  GLuint vs_handle = _compile(vs_source, Shader::VERTEX_SHADER);
  GLuint fs_handle = _compile(fs_source, Shader::FRAGMENT_SHADER);
  GLuint handle = glCreateProgram();
  glAttachShader(handle, vs_handle);
  glAttachShader(handle, fs_handle);
  glLinkProgram(handle);
  glDeleteShader(vs_handle);
  glDeleteShader(fs_handle);
  GLint success;
  glGetProgramiv(handle, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar buf[512];
    glGetProgramInfoLog(handle, (GLuint)512, NULL, buf);
    std::cerr << "ShaderLinkError: " + std::string(buf)
              << std::endl;  // TODO: proper exception
  }
  return handle;
}

//
// protected
//

void Shader::load(const std::string& vs_path, const std::string& fs_path) {
  if (bound) {
    destroy();
    std::cerr << "ShaderWarning: bound program replaced."
              << std::endl;  // TODO: proper warnings
  }
  const std::string vs_source = io::text::read(vs_path);
  const std::string fs_source = io::text::read(fs_path);
  handle = _attach(vs_source, fs_source);
  bound = true;
}

void Shader::attach(const std::string& vs_source,
                    const std::string& fs_source) {
  if (bound) {
    destroy();
    std::cerr << "ShaderWarning: bound program replaced."
              << std::endl;  // TODO: proper warnings
  }
  handle = _attach(vs_source, fs_source);
  bound = true;
}

//
// public
//

bool Shader::input::operator==(const Shader::input& rhs) const {
  return length == rhs.length && gl_type == rhs.gl_type;
}

Shader::Shader() {}

Shader::~Shader() { destroy(); }

void Shader::destroy() {
  // TODO: check if handle is valid
  glDeleteProgram(handle);
}

void Shader::use() {
  // TODO: check if handle is valid
  glUseProgram(handle);
}
