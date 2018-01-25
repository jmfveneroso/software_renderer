#include "input.hpp"

namespace Sibyl {

Input::Input(
  std::shared_ptr<Window> window,
  std::shared_ptr<Player> player,
  std::shared_ptr<EntityManager> entity_manager 
) : window_(window), player_(player), entity_manager_(entity_manager) {
}

void Input::ProcessInput(){
  static double last_time = glfwGetTime();
  
  // Compute time difference between current and last frame
  double current_time = glfwGetTime();
  float delta_time = float(current_time - last_time);

  player_->set_last_position(player_->position());
  if (glfwGetKey(window_->window(), GLFW_KEY_W) == GLFW_PRESS)
    player_->Move(FORWARD, delta_time);

  // Move backward
  if (glfwGetKey(window_->window(), GLFW_KEY_S) == GLFW_PRESS)
    player_->Move(BACK, delta_time);

  // Strafe right
  if (glfwGetKey(window_->window(), GLFW_KEY_D) == GLFW_PRESS)
    player_->Move(RIGHT, delta_time);

  // Strafe left
  if (glfwGetKey(window_->window(), GLFW_KEY_A) == GLFW_PRESS)
    player_->Move(LEFT, delta_time);

  if (glfwGetKey(window_->window(), GLFW_KEY_Q) == GLFW_PRESS) {
    entity_manager_->GetTerrain()->Erode();
  }

  if (glfwGetKey(window_->window(), GLFW_KEY_SPACE) == GLFW_PRESS)
    player_->Jump();

  double x_pos, y_pos;
  glfwGetCursorPos(window_->window(), &x_pos, &y_pos);
  glfwSetCursorPos(window_->window(), 1024 / 2, 768 / 2);
  player_->ChangeOrientation(x_pos, y_pos);

  last_time = current_time;
}

} // End of namespace.
