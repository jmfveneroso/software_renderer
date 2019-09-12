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
  static bool on_g;
  static bool on_delete;
  static double repeat_wait;
  static bool ignore;
  static int mode;
  static vector<string> content_;
  static string command;
  static bool enabled;
  static double debounce_timer;
  static string write_buffer;
  static int cursor_row_;
  static int cursor_col_;
  static TextEditor& GetInstance();
  static void PressCharCallback(GLFWwindow*, unsigned);
  static void PressKeyCallback(GLFWwindow*, int, int, int, int);
  static double cursor_debounce_timer_;
  static double cursor_timer;
  static string filename;
  static int start_line;

  TextEditor() {}
  static void Draw();
  static void SetContent(string);
  static void OpenFile(string);
  static void WriteFile();
  static void Enable() { enabled = true; cursor_row_ = 0; cursor_col_ = 0; }
  static bool Close() { return !enabled; }
};

} // End of namespace.

#endif
