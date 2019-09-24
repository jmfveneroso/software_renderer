#include "game_state.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

string GameState::write_buffer_;
queue<KeyPress> GameState::input_queue_;

GameState::GameState() {
  Init();
}

void GameState::Init() {
  if (!glfwInit()) throw "Failed to initialize GLFW";

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // To make MacOS happy; should not be needed.
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

  if (FULLSCREEN)
    window_ = glfwCreateWindow(window_width_, window_height_, APP_NAME, glfwGetPrimaryMonitor(), NULL);
  else
    window_ = glfwCreateWindow(window_width_, window_height_, APP_NAME, NULL, NULL);

  if (window_ == NULL) {
    glfwTerminate();
    throw "Failed to open GLFW window";
  }

  // We would expect width and height to be 1024 and 768
  // But on MacOS X with a retina screen it'll be 1024*2 and 768*2, 
  // so we get the actual framebuffer size:
  glfwGetFramebufferSize(window_, &window_width_, &window_height_);
  glfwMakeContextCurrent(window_);

  // Needed for core profile.
  glewExperimental = true; 
  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    throw "Failed to initialize GLEW";
  }

  // Hide the mouse and enable unlimited movement.
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwPollEvents();
  glfwSetCursorPos(window_, window_width_ / 2, window_height_ / 2);

  glfwSetCharCallback(window_, GameState::PressCharCallback);
  glfwSetKeyCallback(window_, GameState::PressKeyCallback);

  projection_matrix_ = glm::perspective(glm::radians(PLAYER_FOV), 4.0f / 3.0f, NEAR_CLIPPING, FAR_CLIPPING);
}

void GameState::PressCharCallback(GLFWwindow* window, unsigned int char_code) {
  write_buffer_ += (char) char_code;
}

void GameState::PressKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  input_queue_.push({ key, scancode, action, mods });
}

bool GameState::ReadBuffer(string* buffer) {
  if (!write_buffer_.size()) {
    return false;
  }

  *buffer = write_buffer_;
  write_buffer_ = "";
  return true;
}

bool GameState::ReadKeyPress(KeyPress* kp) {
  if (input_queue_.empty()) {
    return false;
  }

  *kp = input_queue_.front();
  input_queue_.pop();
  return true;
}

bool GameState::ChangeMode(GameMode mode) {
  mode_ = mode;
  while (!input_queue_.empty()) input_queue_.pop();
  write_buffer_ = "";
  return false;
}

void GameState::UpdateViewMatrix() {
  glm::vec3 direction(
    cos(player_.v_angle) * sin(player_.h_angle), 
    sin(player_.v_angle),
    cos(player_.v_angle) * cos(player_.h_angle)
  );
  
  right_ = glm::vec3(
    sin(player_.h_angle - 3.14f/2.0f), 
    0,
    cos(player_.h_angle - 3.14f/2.0f)
  );

  front_ = glm::vec3(
    cos(player_.v_angle) * sin(player_.h_angle), 
    0,
    cos(player_.v_angle) * cos(player_.h_angle)
  );
  
  glm::vec3 up = glm::cross(right_, direction);

  camera_.position = player_.position;
  camera_.direction = direction;
  camera_.up = up;

  // Camera matrix
  view_matrix_ = glm::lookAt(
    camera_.position,                    // Camera is here
    camera_.position + camera_.direction, // and looks here : at the same position, plus "direction"
    camera_.up                           // Head is up (set to 0,-1,0 to look upside-down)
  );
}

void GameState::MovePlayer(Direction direction) {
  switch (direction) {
    case FORWARD:
      player_.speed += front_ * PLAYER_SPEED;
      break;
    case BACK:
      player_.speed -= front_ * PLAYER_SPEED;
      break;
    case RIGHT:
      player_.speed += right_ * PLAYER_SPEED;
      break;
    case LEFT:
      player_.speed -= right_ * PLAYER_SPEED;
      break;
    default:
      break;
  }
}

void GameState::Look(Direction direction, GLfloat amount) {
  switch (direction) {
    case LEFT:
      player_.h_angle += amount;
      break;
    case DOWN:
      player_.v_angle -= amount;
      break;
    case UP:
      player_.v_angle += amount;
      break;
    case RIGHT:
      player_.h_angle -= amount;
      break;
    default:
      break;
  }
}

void GameState::Jump() {
  if (player_.can_jump) {
    player_.can_jump = false;
    player_.speed.y += 0.3f;
  }
}

} // End of namespace.
