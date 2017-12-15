#include "input.hpp"

namespace Sibyl {

Input::Input(std::shared_ptr<Window> window)
  : window_(window), 
    over_ground_(false),
    position_(glm::vec3(0.0f, 1000.0f, 5.0f)),
    horizontal_angle_(3.14f),
    vertical_angle_(0.0f),
    initial_fov_(45.0f),
    speed_(15.0f),
    fall_speed_(glm::vec3(0.0f, 0.0f, 0.0f)),
    mouse_speed_(0.005f) {
}

void Input::UpdateGravity() {
  if (over_ground_) fall_speed_ = glm::vec3(0, 0, 0);
  else {
    if (position_.y > 11.2f) {
      fall_speed_ += glm::vec3(0, -0.01, 0);
    } else {
      fall_speed_ += glm::vec3(0, -0.003, 0);
    }
  }
  if (glm::length2(fall_speed_) > 3) fall_speed_ = glm::vec3(0, -3.0f, 0);

  position_ += fall_speed_;
  if (position_.y < -30.0f) {
    position_.y = -30.0f;
    over_ground_ = true;
  } else {
    over_ground_ = false;
  }
}

void Input::ProcessInput(){
  // glfwGetTime is called only once, the first time this 
  // function is called.
  static double last_time = glfwGetTime();
  
  // Compute time difference between current and last frame.
  double current_time = glfwGetTime();
  float delta_time = float(current_time - last_time);
  
  // Reset mouse position for next frame
  double xpos, ypos;
  glfwGetCursorPos(window_->window(), &xpos, &ypos);
  glfwSetCursorPos(window_->window(), 1024/2, 768/2);
  
  // Compute new orientation
  horizontal_angle_ += mouse_speed_ * float(1024/2 - xpos);
  vertical_angle_   += mouse_speed_ * float(768/2 - ypos);
  if (vertical_angle_ < -1.57f) vertical_angle_ = -1.57f;
  if (vertical_angle_ > 1.57f)  vertical_angle_ = 1.57f;
  
  // Direction : Spherical coordinates to Cartesian coordinates conversion.
  glm::vec3 direction(
    cos(vertical_angle_) * sin(horizontal_angle_), 
    sin(vertical_angle_),
    cos(vertical_angle_) * cos(horizontal_angle_)
  );
  
  // Right vector.
  glm::vec3 right = glm::vec3(
    sin(horizontal_angle_ - 3.14f/2.0f) * 2, 
    0,
    cos(horizontal_angle_ - 3.14f/2.0f) * 2
  );

  // Front vector.
  glm::vec3 front = glm::vec3(
    sin(horizontal_angle_) * 2, 
    0,
    cos(horizontal_angle_) * 2
  );
  
  // Up vector.
  glm::vec3 up = glm::cross(right, direction);
  
  // Move forward.
  if (glfwGetKey(window_->window(), GLFW_KEY_W) == GLFW_PRESS)
    position_ += front * delta_time * speed_;

  // Move backward
  if (glfwGetKey(window_->window(), GLFW_KEY_S) == GLFW_PRESS)
    position_ -= front * delta_time * speed_;

  // Strafe right
  if (glfwGetKey(window_->window(), GLFW_KEY_D) == GLFW_PRESS)
    position_ += right * delta_time * speed_;

  // Strafe left
  if (glfwGetKey(window_->window(), GLFW_KEY_A) == GLFW_PRESS)
    position_ -= right * delta_time * speed_;

  if (glfwGetKey(window_->window(), GLFW_KEY_SPACE) == GLFW_PRESS) {
    if (over_ground_ || position_.y < 11.2f) {
      if (position_.y < 11.2f)
        fall_speed_ = glm::vec3(0, 0.3f, 0);
      else
        fall_speed_ = glm::vec3(0, 0.5f, 0);
      over_ground_ = false;
    }
  }
  
  // For the next frame, the "last time" will be "now".
  last_time = current_time;
}

} // End of namespace.
