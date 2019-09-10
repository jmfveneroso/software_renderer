#ifndef _GRAPHICS_HPP_
#define _GRAPHICS_HPP_

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

class Mesh {
  Shader shader_;
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint normal_buffer_;
  GLuint element_buffer_;
  std::vector<glm::vec3> vertices_;
  std::vector<glm::vec3> normals_;
  std::vector<unsigned int> indices_;

  void Load(const string& filename);

 public:
  Mesh() {}
  Mesh(const string&);

  void Draw(glm::mat4, glm::mat4, glm::vec3, glm::vec3, GLfloat);
};

class Graphics {
  Shader shader_;
  GLuint vbo_;
  unordered_map<string, Shader> shaders_;
  unordered_map<string, GLuint> vbos_;
  unordered_map<string, Mesh> meshes_;
  unordered_map<GLchar, Character> characters_;
  glm::mat4 projection_;

  void CreateShaders();
  void CreateVBOs();
  void LoadFonts();
  void LoadMeshes();
  void DrawChar(char, float, float, vec3 = {1.0, 1.0, 1.0});

 public:
  Graphics();
  Graphics(Graphics const&) = delete;
  void operator=(Graphics const&) = delete;
  static Graphics& GetInstance();

  inline void set_projection(
    const glm::mat4& projection = glm::ortho(0.0f, (float) WINDOW_WIDTH, 0.0f, (float) WINDOW_HEIGHT)
  ) { 
    projection_ = projection; 
  }
  void DrawText(const string&, float, float, vec3 = {1.0, 1.0, 1.0});
  void DrawMesh(string, glm::mat4, glm::mat4, glm::vec3, glm::vec3, GLfloat);
  void Rectangle(GLfloat, GLfloat, GLfloat, GLfloat, vec3);
  void Cube(mat4, mat4, vec3, vec3, vec3, GLfloat);
};

} // End of namespace.

#endif
