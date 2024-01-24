//
// private
//

inline u32 _stride(const std::vector<attribute>& structure) {
  u32 stride = 0;
  for (const attribute& data : structure) {
    stride += data.length * sizeof(data.gl_type);
  }
  return stride;
}

//
// public
//

template <typename T>
VAO<T>::VAO(const std::vector<T>& buffer,
            const std::vector<attribute>& structure)
    : buffer(buffer), structure(structure) {};

template <typename T>
VAO<T>::~VAO() {
  destroy();
}

template <typename T>
void VAO<T>::destroy() {
  if (!_is_loaded) {
    return;
  }
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  _is_loaded = false;
}

template <typename T>
void VAO<T>::load() {
  if (_is_loaded) {
    return;
  }
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER,
               buffer.size() * sizeof(buffer[0]),
               &buffer[0],
               GL_STATIC_DRAW);
  u32 start = 0, stride = _stride(structure);
  for (u32 i = 0; i < structure.size(); ++i) {
    glVertexAttribPointer(i,
                          structure[i].length,
                          structure[i].gl_type,
                          structure[i].normalize,
                          stride,
                          (void*)start);
    glEnableVertexAttribArray(i);
    start += structure[i].length * sizeof(structure[i].gl_type);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  _is_loaded = true;
};

template <typename T>
bool VAO<T>::is_loaded() {
  return _is_loaded;
}

template <typename T>
void VAO<T>::draw() {
  if (!_is_loaded) {
    load();
  }
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, buffer.size());
  glBindVertexArray(0);
}
