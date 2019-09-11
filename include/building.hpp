#ifndef _BUILDING_HPP_
#define _BUILDING_HPP_

#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <cstring>
#include <sstream>
#include <streambuf>
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
#include "graphics.hpp"
#include "text_editor.hpp"
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
  glm::vec3 position_;
  GLfloat rotation_;
  string mesh_name_;

 public:
  Object() {}
  Object(glm::vec3, GLfloat, const string&);

  void Draw(glm::mat4, glm::mat4, glm::vec3);
};

class Scroll {
  Object object_;

 public:
  glm::vec3 position;
  string filename;

  Scroll(glm::vec3, GLfloat, const string&);
  
  Object& object() { return object_; }
};

class Floor {
  glm::vec3 position_;
  float width_;
  float height_;
  float length_;

 public:
  Floor(glm::vec3, float, float, float);

  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void Collide(glm::vec3&, glm::vec3, bool&, glm::vec3&);
};

class Building {
  double debounce_timer_ = glfwGetTime();
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint element_buffer_;
  GLuint quad_vbo_;
  std::vector<glm::vec3> vertices_;
  std::vector<unsigned int> indices_;
  float sx_, sz_;
  std::string opened_document_;

  std::vector<Floor> floors_;
  vector<WallPainting> paintings_;
  std::vector<Object> objects_;
  std::vector<Scroll> scrolls_;

 protected:
  glm::vec3 position_;
  Shader shader_;
  Shader shader2_;

  void CreateFloor(glm::vec3, float, bool);

 public:

  Building(float, float, glm::vec3);

  void Interact(Player&, GameState&);
  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void Collide(glm::vec3&, glm::vec3, bool&, glm::vec3&);
};

} // End of namespace.

#endif
