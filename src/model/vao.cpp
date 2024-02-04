#include "vao.h"

//
// public
//

VAO::VAO(std::shared_ptr<void> raw, u32 size,
         const std::vector<VAO::attribute>& structure)
    : raw_(raw), size_(size), structure_(structure) {
  stride_ = VAO::stride(structure);
  length_ = size / stride_;
};

VAO::~VAO() { destroy(); }

void VAO::load() {
  if (is_loaded_) {
    return;
  }
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, size_, raw_.get(), GL_STATIC_DRAW);
  u32 start = 0;
  for (u32 i = 0; i < structure_.size(); ++i) {
    glVertexAttribPointer(i, structure_[i].format.length,
                          structure_[i].format.gl_type, structure_[i].normalize,
                          stride_, (void*)start);
    glEnableVertexAttribArray(i);
    start += structure_[i].format.length * sizeof(structure_[i].format.gl_type);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  is_loaded_ = true;
};

bool VAO::is_loaded() { return is_loaded_; }

void VAO::destroy() {
  if (!is_loaded_) {
    return;
  }
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
  is_loaded_ = false;
}

void VAO::draw() {
  if (!is_loaded_) {
    load();
  }
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLES, 0, length_);
  glBindVertexArray(0);
}
