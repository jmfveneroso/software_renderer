#ifndef _TERMINAL_HPP_
#define _TERMINAL_HPP_

#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <cstring>
#include <sstream>
#include <unordered_map>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "graphics.hpp"
#include "shaders.h"
#include "config.h"

namespace Sibyl {

class Terminal {
  vector<string> lines_;

 protected:
  Shader shader_;
  Shader text_shader_;

 public:
  static bool enabled;
  static double debounce_timer;
  static string write_buffer;
  static void PressKey(GLFWwindow*, unsigned);

  Terminal();
  void Draw(glm::vec3);
  void Update();

  void Backspace();
  void Write(std::string);
  void NewLine(bool);
  void Execute(GameState&, Player&);
  void Clear();
  bool Move(Player&, vector<string>&);

  bool SetState(bool state) { 
    double current_time = glfwGetTime();
    if (current_time <= debounce_timer) {
      debounce_timer = current_time + DEBOUNCE_DELAY; 
      return false;
    }
    
    debounce_timer = current_time + DEBOUNCE_DELAY; 
    enabled = state;
    Clear();
    NewLine(true);
    return true;
  }
};

} // End of namespace.

#endif
