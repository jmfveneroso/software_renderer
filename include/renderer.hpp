#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

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

struct Mesh {
  Shader shader_;
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint normal_buffer_;
  GLuint element_buffer_;
  std::vector<glm::vec3> vertices_;
  std::vector<glm::vec2> uvs_;
  std::vector<glm::vec3> normals_;
  std::vector<unsigned int> indices_;

  Mesh() {}
};

struct FBO {
  GLuint framebuffer;
  GLuint texture;
  GLuint width;
  GLuint height;
  GLuint depth_rbo;
};

class Renderer {
  unordered_map<string, Shader> shaders_;
  unordered_map<GLchar, Character> characters_;
  unordered_map<string, GLuint> textures_;
  unordered_map<string, FBO> fbos_;

  Shader shader_;
  GLuint text_vbo_;
  GLuint vbo_;
  unordered_map<string, GLuint> vbos_;
  unordered_map<string, Mesh> meshes_;
  glm::mat4 projection_;

  void CreateShaders();
  void CreateVBOs();
  void LoadFonts();
  void LoadMeshes();

 public:
  Renderer();
  Renderer(Renderer const&) = delete;
  void operator=(Renderer const&) = delete;
  static Renderer& GetInstance();

  inline void set_projection(
    const glm::mat4& projection = glm::ortho(0.0f, (float) WINDOW_WIDTH, 0.0f, (float) WINDOW_HEIGHT)
  ) { 
    projection_ = projection; 
  }

  void CreateFramebuffer(const string&, int, int);
  void DrawChar(char, float, float, vec3 = {1.0, 1.0, 1.0}, GLfloat = 1.0);
  void DrawText(const string&, float, float, vec3 = {1.0, 1.0, 1.0}, GLfloat = 1.0);
  void DrawMesh(string, glm::mat4, glm::mat4, glm::vec3, glm::vec3, GLfloat, bool);
  void DrawRectangle(GLfloat, GLfloat, GLfloat, GLfloat, vec3);
  void DrawCube(mat4, mat4, vec3, vec3, vec3, GLfloat);
  void DrawPoint(vec2, GLfloat, vec3);
  void DrawLine(vec2, vec2, GLfloat, vec3);
  void DrawArrow(vec2, vec2, GLfloat, vec3);
  void DrawOneDimensionalSpace(int, int, int);
  void DrawCartesianGrid(int, int, int);
  void LoadMesh(const string&);
  void LoadMesh(const string&, vector<glm::vec3>&, vector<glm::vec2>&, vector<unsigned int>&);
  void LoadMesh(const string&, vector<glm::vec3>&, vector<glm::vec2>&, vector<glm::vec3>&, vector<unsigned int>&);
  void DrawHighlightedObject(string, mat4, mat4, vec3, vec3, GLfloat, bool, GLuint, GLfloat alpha = 1.0);
  FBO GetFBO(const string& name) { return fbos_[name]; }

  void SetFBO(const string& name) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbos_[name].framebuffer);
    glViewport(0, 0, fbos_[name].width, fbos_[name].height);
  }

  void Clear(GLfloat r, GLfloat g, GLfloat b) {
    glClearColor(r, g, b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void DrawScreen(bool);
};

} // End of namespace.

#endif
