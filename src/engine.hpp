#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <vector>
#include <fstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;
#include "shaders.h"
#include "bitmap.h"
#include "tangentspace.h"
#include "objloader.h"
#include "vbo_indexer.h"
#include "render_object.h"
#include "water.h"
#include "renderer.h"

class Engine {
  GLFWwindow* window_;
  const char* window_name_;
  int window_width_, window_height_;

 public:
  Engine() : window_name_("Test"), window_width_(600), window_height_(400) {}

  int CreateWindow();
  int Run();
};

#endif
