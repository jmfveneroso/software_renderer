#ifndef _ROCK_HPP_
#define _ROCK_HPP_

#include <fstream>
#include <vector>
#include <iostream>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include "geometry.hpp"
#include "entity.hpp"
#include "config.h"

namespace Sibyl {

class Rock : public IEntity {
  int num_circles_;
  int num_points_in_circle_;
  int dome_radius_;
  glm::vec3 rock_color_;

  SimplexNoise noise_;
  glm::vec3 position_;
  Shader shader_;
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint normal_buffer_;
  GLuint element_buffers_[5];
  unsigned int sizes_[5];

  std::vector<glm::vec3> vertices_;
  std::vector<glm::vec3> normals_;
  std::vector<glm::vec2> uvs_;
  std::vector<unsigned int> indices_;
  GLuint texture_;

  void Init();
  void Flatten();
  void CalculateNormals();
  void CalculateIndices(int);
  void Smooth();

 public:
  Rock(Shader, GLuint);

  void DrawShit(glm::mat4, glm::mat4, glm::mat4, glm::vec3, int);
  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void Clean() {}
  std::vector<glm::vec3> vertices() { return std::vector<glm::vec3>(); }
  void set_position(glm::vec3 position) { position_ = position; }
};

} // End of namespace.

#endif
