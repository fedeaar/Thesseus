#include "camera.h"

//
// private
//

inline v3f
direction_(f32 yaw, f32 pitch)
{
  return glm::normalize(
    (v3f){ cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
           sin(glm::radians(pitch)),
           sin(glm::radians(yaw)) * cos(glm::radians(pitch)) });
}

//
// public
//

Camera::Camera(f32 aspect,
               f32 fov,
               f32 speed,
               f32 sensitivity,
               f32 yaw,
               f32 pitch,
               const v3f& position)
  : aspectRatio_(aspect)
  , fov_(fov)
  , baseSpeed_(speed)
  , speed_(speed)
  , sensitivity_(sensitivity)
  , yaw_(yaw)
  , pitch_(pitch)
  , position_(position)
  , front_(direction_(yaw, pitch))
  , up_({ 0.0f, 1.0f, 0.0f })
{
  viewMatrix_ = glm::lookAt(position_, position_ + front_, up_);
  projMatrix_ =
    glm::perspective(glm::radians(fov_), aspectRatio_, 0.1f, 100.0f);
}

void
Camera::move(Camera::Movement type)
{
  switch (type) {
    case TOWARDS:
      position_ += speed_ * front_;
      break;
    case AGAINST:
      position_ -= speed_ * front_;
      break;
    case UPWARDS:
      position_ -= speed_ * up_;
      break;
    case DOWNWARDS:
      position_ += speed_ * up_;
      break;
    case RIGHT:
      position_ += speed_ * glm::normalize(glm::cross(front_, up_));
      break;
    case LEFT:
      position_ -= speed_ * glm::normalize(glm::cross(front_, up_));
      break;
  }
  viewMatrix_ = glm::lookAt(position_, position_ + front_, up_);
}

void
Camera::rotate(Camera::Rotation type, f32 angle)
{
  switch (type) {
    case YAW:
      yaw_ += angle * sensitivity_;
      break;
    case PITCH:
      pitch_ = core::clamp(pitch_ + angle * sensitivity_, -90.f, 90.f);
      break;
  }
  front_ = direction_(yaw_, pitch_);
  viewMatrix_ = glm::lookAt(position_, position_ + front_, up_);
}

void
Camera::set_speed(f32 speed)
{
  baseSpeed_ = speed;
}

void
Camera::set_aspect(f32 aspect)
{
  aspectRatio_ = aspect;
  projMatrix_ =
    glm::perspective(glm::radians(fov_), aspectRatio_, 0.1f, 100.0f);
}

void
Camera::set_frame_delta(f32 tick_delta)
{
  f32 frame_delta = (1 - tick_delta);
  speed_ = baseSpeed_ * frame_delta;
}

const v3f&
Camera::position() const
{
  return position_;
}

const m4f&
Camera::view_matrix() const
{
  return viewMatrix_;
}

const m4f&
Camera::proj_matrix() const
{
  return projMatrix_;
}
