#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include <iostream>
#include <exception>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include <exception>
#include <memory>
#include <string>
#include <queue>
#include "config.h"

using namespace std;
using namespace glm;

namespace Sibyl {

enum GameMode {
  FREE,
  TXT
};

enum Direction {
  FORWARD,
  BACK,
  LEFT,
  RIGHT,
  UP,
  DOWN
};

struct KeyPress {
  int key; 
  int scancode; 
  int action; 
  int mods;
};

struct Camera {
  glm::vec3 position;
  glm::vec3 up;
  glm::vec3 direction;
};

struct Player {
  glm::vec3 position = glm::vec3(2002.5, 208, 1985);
  glm::vec3 next_position = glm::vec3(0, 0, 0);
  glm::vec3 speed = glm::vec3(0, 0, 0);
  float h_angle = 0.0f;
  float v_angle = 0.0f;
  float height = PLAYER_HEIGHT;
  bool can_jump = false;
};

class GameState {
  static string write_buffer_;
  static queue<KeyPress> input_queue_;
  static void PressCharCallback(GLFWwindow*, unsigned int);
  static void PressKeyCallback(GLFWwindow*, int, int, int, int);

  GLFWwindow* window_;
  int window_width_ = WINDOW_WIDTH;
  int window_height_ = WINDOW_HEIGHT;
  Camera camera_;
  Player player_;
  glm::vec3 right_;
  glm::vec3 front_;

  GameMode mode_ = FREE;
  mat4 projection_matrix_;
  mat4 view_matrix_;

 public:
  GameState();
  GLFWwindow* window() { return window_; }
  int width() { return window_width_; }
  int height() { return window_height_; }

  void Init();
  bool ReadBuffer(string*);
  bool ReadKeyPress(KeyPress*);
  bool ChangeMode(GameMode);
  void UpdateViewMatrix();
  void UpdatePlayerPosition();
  void MovePlayer(Direction);
  void Look(Direction, GLfloat);
  void Jump();

  Camera camera() { return camera_; }
  GameMode mode() { return mode_; }
  mat4 projection_matrix() { return projection_matrix_; }
  mat4 view_matrix() { return view_matrix_; }
  Player& player() { return player_; }

  void set_h_angle(GLfloat h_angle) { player_.h_angle = h_angle; }
  void set_v_angle(GLfloat v_angle) { player_.v_angle = v_angle; }
};

} // End of namespace.

#endif
