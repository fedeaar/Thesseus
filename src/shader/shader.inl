inline void Shader::set_uniform(const std::string& ref, bool val) {
  // TODO: error checking
  glUniform1i(glGetUniformLocation(handle_, ref.c_str()), (int)val);
}

inline void Shader::set_uniform(const std::string& ref, u32 val) {
  // TODO: error checking
  glUniform1ui(glGetUniformLocation(handle_, ref.c_str()), val);
}

inline void Shader::set_uniform(const std::string& ref, i32 val) {
  // TODO: error checking
  glUniform1i(glGetUniformLocation(handle_, ref.c_str()), val);
}

inline void Shader::set_uniform(const std::string& ref, f32 val) {
  // TODO: error checking
  glUniform1f(glGetUniformLocation(handle_, ref.c_str()), val);
}

inline void Shader::set_uniform(const std::string& ref, const v3f& val) {
  // TODO: error checking
  glUniform3f(glGetUniformLocation(handle_, ref.c_str()), val.x, val.y, val.z);
}

inline void Shader::set_uniform(const std::string& ref, const v4f& val) {
  // TODO: error checking
  glUniform4f(glGetUniformLocation(handle_, ref.c_str()), val.x, val.y, val.z,
              val.w);
}

inline void Shader::set_uniform(const std::string& ref, const m3f& val) {
  // TODO: error checking;
  glUniformMatrix3fv(glGetUniformLocation(handle_, ref.c_str()), 1, GL_FALSE,
                     glm::value_ptr(val));
}

inline void Shader::set_uniform(const std::string& ref, const m4f& val) {
  // TODO: error checking;
  glUniformMatrix4fv(glGetUniformLocation(handle_, ref.c_str()), 1, GL_FALSE,
                     glm::value_ptr(val));
}
