#ifndef _CUBE_HPP_
#define _CUBE_HPP_

#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <cstring>
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
#include "clipmap.hpp"

namespace Sibyl {

class Cube {
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint element_buffer_;
  std::vector<glm::vec3> vertices_;
  std::vector<unsigned int> indices_;

  protected:
   glm::vec3 speed_ = glm::vec3(0, 0.1, 0);
   glm::vec3 position_;
   Shader shader_;
   float v_angle_ = 0.0;
   float h_angle_ = 0.0;

   void CreateCube();

 public:
  Cube(Shader shader);

  void Draw(glm::mat4, glm::mat4, glm::vec3);
};

} // End of namespace.

#endif
