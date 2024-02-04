#include "camera.h"

//
// private
//

inline v3f _direction(f32 yaw, f32 pitch) {
  return glm::normalize((v3f) {
    cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
    sin(glm::radians(pitch)),
    sin(glm::radians(yaw)) * cos(glm::radians(pitch))});
}

//
// public
//

Camera::Camera(
  f32 speed,
  f32 fov,
  f32 sensitivity,
  f32 yaw,
  f32 pitch,
  const v3f& position)
    : _speed(speed), _fov(fov), _sensitivity(sensitivity), _yaw(yaw),
      _pitch(pitch), _position(position), _front(_direction(yaw, pitch)),
      _up({0.0f, 0.1f, 0.0f}) {};

m4f Camera::view_matrix() {
  return glm::lookAt(_position, _position + _front, _up);
}

m4f Camera::proj_matrix(f32 aspect) {
  return glm::perspective(glm::radians(_fov), aspect, 0.1f, 100.0f);
}

void Camera::move(Camera::Movement type) {
  switch (type) {
  case TOWARDS:
    _position += _speed * _front;
    break;
  case AGAINST:
    _position -= _speed * _front;
    break;
  case UPWARDS:
    _position += _speed * _up;
    break;
  case DOWNWARDS:
    _position -= _speed * _up;
    break;
  case RIGHT:
    _position += _speed * glm::normalize(glm::cross(_front, _up));
    break;
  case LEFT:
    _position -= _speed * glm::normalize(glm::cross(_front, _up));
    break;
  }
}

void Camera::rotate(Camera::Rotation type, f32 angle) {
  switch (type) {
  case YAW:
    _yaw += angle * _sensitivity;
    break;
  case PITCH:
    _pitch = clamp(_pitch - angle * _sensitivity, -90.f, 90.f);
    break;
  }
  _front = _direction(_yaw, _pitch);
}

void Camera::set_speed(f32 speed) {
  _speed = speed;
}
