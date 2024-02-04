//
// public
//

template <typename T>
Model<T>::Model(std::shared_ptr<VAO> vao, v3f position, f32 angle, v3f axis)
    : vao(vao), position(position), angle(angle), axis(axis) {}

template <typename T>
Model<T>::~Model() {
  if (vao.unique()) {
    vao->destroy();
  }
}

template <typename T>
void Model<T>::rotate(f32 new_angle, v3f new_axis) {
  axis = new_axis;
  angle = new_angle;
}

template <typename T>
void Model<T>::place(v3f new_position) {
  position = new_position;
}

template <typename T>
void Model<T>::render(T& program) {
  glm::mat4 transform = m4f(1.0f);
  transform = glm::translate(transform, position);
  transform = glm::rotate(transform, angle, axis);
  program.set_model(transform);
  vao->draw();
}
