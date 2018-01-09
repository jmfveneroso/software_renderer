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
    sin(horizontal_angle_ - 3.14f/2.0f) * 2, 
    0,
    cos(horizontal_angle_ - 3.14f/2.0f) * 2
  );

  glm::vec3 front = glm::vec3(
    sin(horizontal_angle_) * 2, 
    0,
    cos(horizontal_angle_) * 2
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
  speed_.y += 0.3f;
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

} // End of namespace.
