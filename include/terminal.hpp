#ifndef _TERMINAL_HPP_
#define _TERMINAL_HPP_

#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <cstring>
#include <sstream>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include "shaders.h"
#include "config.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Sibyl {

class Terminal {
  GLuint vertex_buffer_;
  GLuint element_buffer_;
  std::vector<glm::vec3> vertices_;
  std::vector<unsigned int> indices_;

 protected:
  Shader shader_;

 public:
  bool enabled = false;
  int delay = 0;

  Terminal(Shader);

  void Draw();
};

} // End of namespace.

#endif
