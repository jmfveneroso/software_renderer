#ifndef _BUILDING_HPP_
#define _BUILDING_HPP_

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

class Building {
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint element_buffer_;
  std::vector<glm::vec3> vertices_;
  std::vector<unsigned int> indices_;
  float sx_, sz_;

  protected:
   glm::vec3 speed_ = glm::vec3(0, 0.1, 0);
   glm::vec3 position_;
   Shader shader_;

   void CreateCube();

 public:
  Building(Shader shader, float, float, glm::vec3);

  void Draw(glm::mat4, glm::mat4, glm::vec3);
};

} // End of namespace.

#endif
