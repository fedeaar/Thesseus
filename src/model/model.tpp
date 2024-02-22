//
// public
//

template <typename T>
Model<T>::Model(std::shared_ptr<VAO> vao) : vao_(vao) {}

template <typename T>
Model<T>::~Model() {
  if (vao_.unique()) {
    vao_->destroy();
  }
}

template <typename T>
void Model<T>::use_texture(const std::shared_ptr<Texture2D>& texture,
                           u8 position) {
  textures_[position] = texture;
};

template <typename T>
void Model<T>::rotate(f32 angle, v3f axis) {
  axis_ = axis;
  angle_ = angle;
}

template <typename T>
void Model<T>::place(v3f position) {
  position_ = position;
}

template <typename T>
void Model<T>::scale(v3f scale) {
  scale_ = scale;
}

template <typename T>
void Model<T>::render(T& shader) {
  glm::mat4 transform = m4f(1.0f);
  transform = glm::translate(transform, position_);
  transform = glm::rotate(transform, angle_, axis_);
  transform = glm::scale(transform, scale_);
  glm::mat3 normal = glm::transpose(glm::inverse((m3f)transform));
  shader.use();
  shader.set_model(transform);
  shader.set_normal_matrix(normal);
  vao_->draw();
}
