#include "player.hpp"

namespace Sibyl {

Player::Player()
  : position_(glm::vec3(0, 300, 0)),
    speed_(glm::vec3(0, 0, 0)),
    horizontal_angle_(0.0f),
    vertical_angle_(0.0f),
    fov_(45.0f),
    mouse_speed_(0.005f),
    over_ground_(false) {
}

void Player::Move(Direction direction, float delta_time) {
  glm::vec3 right = glm::vec3(
    sin(horizontal_angle_ - 3.14f/2.0f), 
    0,
    cos(horizontal_angle_ - 3.14f/2.0f)
  );

  glm::vec3 front = glm::vec3(
    sin(horizontal_angle_), 
    0,
    cos(horizontal_angle_)
  );
  
  switch (direction) {
    case FORWARD:
      position_ += front * delta_time * PLAYER_SPEED;
      break;
    case BACK:
      position_ -= front * delta_time * PLAYER_SPEED;
      break;
    case RIGHT:
      position_ += right * delta_time * PLAYER_SPEED;
      break;
    case LEFT:
      position_ -= right * delta_time * PLAYER_SPEED;
      break;
    default:
      break;
  }
}

void Player::ChangeOrientation(double x_pos, double y_pos) {
  horizontal_angle_ += mouse_speed_ * float(1024 / 2 - x_pos);
  vertical_angle_   += mouse_speed_ * float(768 / 2 - y_pos);
  if (vertical_angle_ < -1.57f) vertical_angle_ = -1.57f;
  if (vertical_angle_ >  1.57f) vertical_angle_ =  1.57f;
}

void Player::Jump() {
  speed_.y += 0.6f;
  // speed_.y += 10.0f;
  // if (over_ground_ || position_.y < 11.2f) {
  //   if (position_.y < 11.2f) {
  //     speed_.y = 0.3f;
  //   } else { 
  //     speed_.y = 0.5f;
  //   }
  //   over_ground_ = false;
  // }
}

Plane Player::GetFrustumPlane(FrustumPlane frustum_plane) {
  glm::vec4 view_direction(
    cos(vertical_angle_) * sin(horizontal_angle_),
    sin(vertical_angle_),
    cos(vertical_angle_) * cos(horizontal_angle_),
    1
  );

  glm::vec3 horizontal_axis = glm::vec3(
    sin(horizontal_angle_ - 3.14f / 2.0f),
    0,
    cos(horizontal_angle_ - 3.14f / 2.0f)
  );

  glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), 3.14f / 2.0f, horizontal_axis);
  glm::vec3 vertical_axis = glm::normalize(glm::vec3(rotation_matrix * view_direction));

  float vertical_fov = glm::radians(0.75f * fov_);
  Plane plane;
  // plane.point = glm::vec3(0, 0, 0);
  plane.point = position_;

  switch (frustum_plane) {
    case FRUSTUM_PLANE_UP:
      plane.vec1 = horizontal_axis;
      rotation_matrix = glm::rotate(glm::mat4(1.0f), vertical_fov, horizontal_axis);
      plane.vec2 = glm::normalize(glm::vec3(rotation_matrix * view_direction));
      rotation_matrix = glm::rotate(glm::mat4(1.0f), vertical_fov - (3.14f / 2.0f), horizontal_axis);
      plane.normal = glm::normalize(glm::vec3(rotation_matrix * view_direction));
      return plane;
    case FRUSTUM_PLANE_DOWN:
      plane.vec1 = horizontal_axis;
      rotation_matrix = glm::rotate(glm::mat4(1.0f), -vertical_fov, horizontal_axis);
      plane.vec2 = glm::normalize(glm::vec3(rotation_matrix * view_direction));
      rotation_matrix = glm::rotate(glm::mat4(1.0f), (3.14f / 2.0f) - vertical_fov, horizontal_axis);
      plane.normal = glm::normalize(glm::vec3(rotation_matrix * view_direction));
      return plane;
    case FRUSTUM_PLANE_LEFT:
      plane.vec1 = vertical_axis;
      rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(fov_), vertical_axis);
      plane.vec2 = glm::normalize(glm::vec3(rotation_matrix * view_direction));
      rotation_matrix = glm::rotate(glm::mat4(1.0f), -glm::radians(fov_), vertical_axis);
      plane.normal = glm::normalize(glm::vec3(rotation_matrix * view_direction));
      return plane;
    case FRUSTUM_PLANE_RIGHT:
      plane.vec1 = vertical_axis;
      rotation_matrix = glm::rotate(glm::mat4(1.0f), -glm::radians(fov_), vertical_axis);
      plane.vec2 = glm::normalize(glm::vec3(rotation_matrix * view_direction));
      rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(fov_), vertical_axis);
      plane.normal = glm::normalize(glm::vec3(rotation_matrix * view_direction));
      return plane;
    default: throw;
  }
}

} // End of namespace.
