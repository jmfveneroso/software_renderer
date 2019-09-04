#ifndef _TERMINAL_HPP_
#define _TERMINAL_HPP_

#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <cstring>
#include <sstream>
#include <unordered_map>
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
#include "shaders.h"
#include "config.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Sibyl {

struct Character {
  GLuint     TextureID; // ID handle of the glyph texture
  glm::ivec2 Size;      // Size of glyph
  glm::ivec2 Bearing;   // Offset from baseline to left/top of glyph
  GLuint     Advance;   // Offset to advance to next glyph
};

class Terminal {
  vector<string> history_;
  vector<string> lines_;
  GLuint vertex_buffer_;
  GLuint element_buffer_;
  std::vector<glm::vec3> vertices_;
  std::vector<unsigned int> indices_;
  unordered_map<GLchar, Character> characters_;
  Character cursor_character_;
  GLuint VAO, VBO;

  void LoadFonts();
  void DrawChar(Character&, float, float, glm::vec3 color = {1.0, 1.0, 1.0});
  void DrawText(const string&, float, float, bool, glm::vec3 color = {1.0, 1.0, 1.0});

 protected:
  Shader shader_;
  Shader text_shader_;

 public:
  static bool enabled;
  static double debounce_timer;
  static string write_buffer;
  static void PressKey(GLFWwindow*, unsigned);

  Terminal(Shader, Shader);
  void Draw(glm::vec3);
  void Update();

  void Backspace();
  void Write(std::string);
  void NewLine(bool);
  void Execute(GameState&, Player&);
  void Clear();
  bool Move(Player&, vector<string>&);

  bool SetState(bool state) { 
    double current_time = glfwGetTime();
    if (current_time <= debounce_timer) {
      debounce_timer = current_time + DEBOUNCE_DELAY; 
      return false;
    }
    
    debounce_timer = current_time + DEBOUNCE_DELAY; 
    enabled = state;
    Clear();
    NewLine(true);
    return true;
  }
};

} // End of namespace.

#endif
