#include "camera.h"

//
// private
//

inline v3f _direction(f32 yaw, f32 pitch) {
  return glm::normalize(
      (v3f){cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))});
}

//
// public
//

Camera::Camera(f32 aspect, f32 fov, f32 speed, f32 sensitivity, f32 yaw,
               f32 pitch, const v3f& position)
    : aspect_ratio_(aspect),
      fov_(fov),
      speed_(speed),
      sensitivity_(sensitivity),
      yaw_(yaw),
      pitch_(pitch),
      position_(position),
      front_(_direction(yaw, pitch)),
      up_({0.0f, 1.0f, 0.0f}) {
  view_matrix_ = glm::lookAt(position_, position_ + front_, up_);
  proj_matrix_ =
      glm::perspective(glm::radians(fov_), aspect_ratio_, 0.1f, 100.0f);
};

void Camera::move(Camera::Movement type) {
  switch (type) {
    case TOWARDS:
      position_ += speed_ * front_;
      break;
    case AGAINST:
      position_ -= speed_ * front_;
      break;
    case UPWARDS:
      position_ += speed_ * up_;
      break;
    case DOWNWARDS:
      position_ -= speed_ * up_;
      break;
    case RIGHT:
      position_ += speed_ * glm::normalize(glm::cross(front_, up_));
      break;
    case LEFT:
      position_ -= speed_ * glm::normalize(glm::cross(front_, up_));
      break;
  }
  view_matrix_ = glm::lookAt(position_, position_ + front_, up_);
}

void Camera::rotate(Camera::Rotation type, f32 angle) {
  switch (type) {
    case YAW:
      yaw_ += angle * sensitivity_;
      break;
    case PITCH:
      pitch_ = clamp(pitch_ - angle * sensitivity_, -90.f, 90.f);
      break;
  }
  front_ = _direction(yaw_, pitch_);
  view_matrix_ = glm::lookAt(position_, position_ + front_, up_);
}

void Camera::set_speed(f32 speed) { speed_ = speed; }

void Camera::set_aspect(f32 aspect) {
  aspect_ratio_ = aspect;
  proj_matrix_ =
      glm::perspective(glm::radians(fov_), aspect_ratio_, 0.1f, 100.0f);
}

const v3f& Camera::position() const { return position_; };

const m4f& Camera::view_matrix() const { return view_matrix_; }

const m4f& Camera::proj_matrix() const { return proj_matrix_; }
