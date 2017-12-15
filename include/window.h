#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <exception>
#include <memory>

#include "config.h"

namespace Sibyl {

class Window {
  GLFWwindow* window_;
  int window_width_, window_height_;

  void CreateWindow();

 public:
  Window();

  int width()  { return window_width_; }
  int height() { return window_height_;}
  GLFWwindow* window() { return window_; }
  void SwapBuffers();
  bool ShouldClose();
  void Close();
};

} // End of namespace.

#endif
