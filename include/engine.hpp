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
#include <exception>

using namespace glm;
#include "shaders.h"
#include "bitmap.h"
#include "tangentspace.h"
#include "objloader.h"
#include "vbo_indexer.h"
#include "render_object.h"
#include "water.h"
#include "renderer.h"

namespace Sibyl {

class Engine {
  Renderer renderer_;
  GLFWwindow* window_;
  const char* window_name_;
  int window_width_, window_height_;

  void Clean();
  void CreateWindow();

 public:
  Engine();

  int Run();
};

} // End of namespace.

#endif
