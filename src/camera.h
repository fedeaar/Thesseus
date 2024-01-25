#ifndef CAMERA_
#define CAMERA_

#include "core/inline.hpp"
#include "core/types.h"
#include "shaders/shader.h"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {

private:
  v3f _position, _front, _up;
  f32 _speed, _fov, _sensitivity;
  f32 _yaw, _pitch;

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

  Camera(
    f32 speed = 0.05f,
    f32 fov = 45.0f,
    f32 sensitivity = 0.1f,
    f32 yaw = -90,
    f32 pitch = 0,
    const v3f& position = {0.0f, 0.0f, 0.0f});

  m4f view_matrix();
  m4f proj_matrix(f32 aspect);

  void move(Movement type);
  void rotate(Rotation type, f32 angle);

  void set_speed(f32 speed);
};

#endif // CAMERA_
