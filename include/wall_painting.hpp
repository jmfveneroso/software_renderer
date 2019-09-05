#ifndef _WALL_PAINTING_HPP_
#define _WALL_PAINTING_HPP_

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
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "texture.hpp"
#include "shaders.h"
#include "config.h"

namespace Sibyl {

class WallPainting {
  GLuint frame_buffer_;
  GLuint texture_;
  // Texture texture_;
  Shader shader_;
  Shader shader2_;
  glm::vec3 position_;
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint element_buffer_;
  std::vector<glm::vec3> vertices_;
  std::vector<unsigned int> indices_;
  double size_ = 2.0f;

  void Init();

 public:
  WallPainting() {}
  WallPainting(glm::vec3);

  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void DrawToTexture();
};

} // End of namespace.

#endif
