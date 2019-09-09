#ifndef _TEXT_HPP_
#define _TEXT_HPP_

#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <unordered_map>
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
#include <ft2build.h>
#include "texture.hpp"
#include "shaders.h"
#include "config.h"
#include FT_FREETYPE_H

namespace Sibyl {

using namespace glm;

struct Character {
  GLuint     TextureID; // ID handle of the glyph texture
  glm::ivec2 Size;      // Size of glyph
  glm::ivec2 Bearing;   // Offset from baseline to left/top of glyph
  GLuint     Advance;   // Offset to advance to next glyph
};

class Text {
  Shader shader_;
  GLuint vbo_;
  unordered_map<GLchar, Character> characters_;
  glm::mat4 projection_;

  void LoadFonts();

 public:
  Text();
  Text(Text const&) = delete;
  void operator=(Text const&) = delete;
  static Text& GetInstance();

  void SetProjection(GLfloat width = WINDOW_WIDTH, GLfloat height = WINDOW_HEIGHT);
  void DrawText(const string&, float, float, vec3 = {1.0, 1.0, 1.0});
  void DrawChar(char, float, float, vec3 = {1.0, 1.0, 1.0});
};

} // End of namespace.

#endif
