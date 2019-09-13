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
#include <boost/regex.hpp>
#include "texture.hpp"
#include "graphics.hpp"
#include "shaders.h"
#include "config.h"

namespace Sibyl {

class WallPainting {
  GLuint frame_buffer_;
  GLuint texture_;
  Shader shader_;
  Shader shader2_;
  glm::vec3 position_;
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint element_buffer_;
  std::vector<glm::vec3> vertices_;
  glm::vec2 texture_size_;
  std::vector<unsigned int> indices_;
  double size_ = 2.0f;
  GLuint vbo;
  GLfloat rotation_;
  string filename_;

  void Init();
  void DrawCartesianGrid(int, int, int);
  vec3 GetColor(string);

 public:
  WallPainting() {}
  WallPainting(string, glm::vec3, GLfloat);

  void LoadFile();
  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void DrawToTexture();
  void DrawLine(glm::vec2, glm::vec2, GLfloat, glm::vec3);
  void DrawArrow(glm::vec2, glm::vec2, GLfloat, glm::vec3);
  void DrawPoint(glm::vec2, GLfloat, glm::vec3);
  void DrawText(string, glm::vec2, glm::vec3);
  void BeginDraw();
  void EndDraw();

  string filename() { return filename_; }
  vec3 position() { return position_; }
};

} // End of namespace.

#endif
