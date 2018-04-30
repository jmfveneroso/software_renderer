#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include "geometry.hpp"

namespace Sibyl {

#define PLAYER_SPEED 30000.0f
// #define PLAYER_SPEED 1500.0f
// #define PLAYER_SPEED 100.0f

enum FrustumPlane {
  FRUSTUM_PLANE_LEFT = 0,
  FRUSTUM_PLANE_RIGHT,
  FRUSTUM_PLANE_UP,
  FRUSTUM_PLANE_DOWN
};

enum Direction {
  FORWARD,
  BACK,
  LEFT,
  RIGHT
};

class Player {
  glm::vec3 position_;
  glm::vec3 last_position_;
  glm::vec3 speed_;
  float horizontal_angle_;
  float vertical_angle_;
  float fov_;
  float mouse_speed_;
  bool over_ground_;
  bool mouse_enabled_ = true;

 public:
  Player();

  void EnableMouse(bool flag) { mouse_enabled_ = flag; }
  void Move(Direction, float);
  void ChangeOrientation(double, double);
  void Jump();
  void ApplyForce(glm::vec3 force) { speed_ += force; }
  void Update() { position_ += speed_; }
  Plane GetFrustumPlane(FrustumPlane);

  glm::vec3 position() { return position_; }
  glm::vec3 last_position() { return last_position_; }
  glm::vec3 speed() { return speed_; }
  float horizontal_angle() { return horizontal_angle_; }
  float vertical_angle() { return vertical_angle_; }
  float fov() { return fov_; }
  float over_ground() { return over_ground_; }
  void set_over_ground(bool over_ground) { over_ground_ = over_ground; }
  void set_position(glm::vec3 position) { position_ = position; }
  void set_speed(glm::vec3 speed) { speed_ = speed; }
  void set_last_position(glm::vec3 position) { last_position_ = position; }
  void set_horizontal_angle(float angle) { horizontal_angle_ = angle; }
  void set_vertical_angle(float angle) { vertical_angle_ = angle; }
};

} // End of namespace.

#endif
