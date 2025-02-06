#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../core/inline.hpp"
#include "../core/types.h"

class Camera
{
private:
  v3f position_, front_, up_;
  f32 base_speed_, speed_, fov_, sensitivity_, aspect_ratio_;
  f32 yaw_, pitch_;
  m4f view_matrix_, proj_matrix_;

public:
  enum Movement
  {
    TOWARDS,
    AGAINST,
    UPWARDS,
    DOWNWARDS,
    LEFT,
    RIGHT
  };
  enum Rotation
  {
    YAW,
    PITCH
  };

  Camera(f32 aspect = 1,
         f32 fov = 45.0f,
         f32 speed = 0.1f,
         f32 sensitivity = 0.3f,
         f32 yaw = -90,
         f32 pitch = 0,
         const v3f& position = { 0.0f, 0.0f, 0.0f });

  void move(Movement type);
  void rotate(Rotation type, f32 angle);

  void set_speed(f32 speed);
  void set_aspect(f32 aspect);
  void set_frame_delta(f32 tick_delta);

  const v3f& position() const;
  const m4f& view_matrix() const;
  const m4f& proj_matrix() const;
};
