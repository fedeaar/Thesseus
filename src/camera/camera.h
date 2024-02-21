#ifndef CAMERA_
#define CAMERA_

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../core/inline.hpp"
#include "../core/types.h"

class Camera {
 private:
  v3f _position, _front, _up;
  f32 _speed, _fov, _sensitivity, _aspect_ratio;
  f32 _yaw, _pitch;

 public:
  enum Movement { TOWARDS, AGAINST, UPWARDS, DOWNWARDS, LEFT, RIGHT };
  enum Rotation { YAW, PITCH };

  Camera(f32 aspect = 1, f32 fov = 45.0f, f32 speed = 0.05f,
         f32 sensitivity = 0.1f, f32 yaw = -90, f32 pitch = 0,
         const v3f& position = {0.0f, 0.0f, 0.0f});

  m4f view_matrix() const;
  m4f proj_matrix() const;

  void move(Movement type);
  void rotate(Rotation type, f32 angle);

  void set_speed(f32 speed);
  void set_aspect(f32 aspect);
};

#endif  // CAMERA_
