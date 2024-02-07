#include "vao.h"

//
// public
//

VAO::VAO(std::shared_ptr<Raw> raw) : raw_(raw){};

VAO::~VAO() { destroy(); }

void VAO::load() {
  if (is_loaded_) {
    return;
  }
  const Raw& raw = *raw_.get();
  const std::vector<Raw::full_attribute>& format = raw.format();
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, raw.size(), raw.data().get(), GL_STATIC_DRAW);
  u32 start = 0;
  for (u32 i = 0; i < format.size(); ++i) {
    glVertexAttribPointer(
        i, format[i].attr.format.length, format[i].attr.format.gl_type,
        !format[i].attr.is_normalized, raw.stride(), (void*)start);
    glEnableVertexAttribArray(i);
    start += format[i].size;
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
  glDrawArrays(GL_TRIANGLES, 0, raw_->length());
  glBindVertexArray(0);
}
