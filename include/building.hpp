#ifndef _BUILDING_HPP_
#define _BUILDING_HPP_

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
#include "wall_painting.hpp"
#include "shaders.h"
#include "config.h"

namespace Sibyl {

struct BoundingBox {
  float x, y, z;
  float width, height, length;

  BoundingBox (
  ) : x(0), y(0), z(0),
      width(0), height(0), length(0) {
  }

  BoundingBox (
    float x, float y, float z,
    float width, float height, float length
  ) : x(x), y(y), z(z),
      width(width), height(height), length(length) {
  }
};

class Object {
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint element_buffer_;
  std::vector<glm::vec3> vertices_;
  std::vector<unsigned int> indices_;

  protected:
   glm::vec3 position_;
   Shader shader_;

   void Load(const string& filename);

 public:
  Object() {}
  Object(Shader shader, glm::vec3, const string&);

  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void Collide(glm::vec3&, glm::vec3, bool&, glm::vec3&);
};

class Floor {
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint element_buffer_;
  std::vector<glm::vec3> vertices_;
  std::vector<unsigned int> indices_;

  protected:
   glm::vec3 position_;
   float width_;
   float height_;
   float length_;
   Shader shader_;

   void Init();

 public:
  Floor(Shader shader, glm::vec3, float, float, float);

  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void Collide(glm::vec3&, glm::vec3, bool&, glm::vec3&);
};

class Building {
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint element_buffer_;
  std::vector<glm::vec3> vertices_;
  std::vector<unsigned int> indices_;
  float sx_, sz_;

  std::vector<Floor> floors_;
  Object platform_;

 protected:
  glm::vec3 position_;
  Shader shader_;
  Shader shader2_;

  void CreateFloor(glm::vec3, float, bool);

 public:
  WallPainting painting_;

  Building(Shader, Shader, float, float, glm::vec3);

  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void Collide(glm::vec3&, glm::vec3, bool&, glm::vec3&);
};

} // End of namespace.

#endif
