#include "terminal.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

Terminal::Terminal(
  Shader shader
) : shader_(shader) {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &element_buffer_);

  vertices_ = {
    { -1, 1, 0 },
    { -1, 0, 0 },
    { 1, 1, 0 },
    { 1, 0, 0 }
  };
  indices_ = { 0, 1, 2, 2, 1, 3 };

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices_.size() * sizeof(unsigned int), 
    &indices_[0], 
    GL_STATIC_DRAW
  );

  // Load font.
  FT_Library ft;
  if (FT_Init_FreeType(&ft))
    cout << "ERROR::FREETYPE: Could not init FreeType Library" << endl;
  
  FT_Face face;
  if (FT_New_Face(ft, "meshes/arial.ttf", 0, &face))
    cout << "ERROR::FREETYPE: Failed to load font" << endl; 

  if (FT_Set_Char_Size(face, 0, 16*64, 300, 300))
    cout << "ERROR::FREETYPE: Failed to set char size" << endl; 
}

void Terminal::Draw() {
  if (!enabled)
    return;

  glUseProgram(shader_.program_id());
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  shader_.BindBuffer(vertex_buffer_, 0, 3);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*) 0);

  glDisable(GL_BLEND);
  shader_.Clear();
}

} // End of namespace.
