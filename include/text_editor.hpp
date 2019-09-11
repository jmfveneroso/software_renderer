#ifndef _TEXT_EDITOR_HPP_
#define _TEXT_EDITOR_HPP_

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

class TextEditor {
 public:
  static bool enabled;
  static double debounce_timer;
  static string write_buffer;
  static void PressKey(GLFWwindow*, unsigned);
  static TextEditor& GetInstance();

  string content_;
  int cursor_row_ = 0;
  int cursor_col_ = 0;
  double cursor_debounce_timer_;
  double draw_cursor = 0.0;

  TextEditor() {}
  void Draw();
  bool Enable(GameState&, bool);
  void MoveCursor(int, int);

  void set_content(string content) { content_ = content; }
};

} // End of namespace.

#endif
