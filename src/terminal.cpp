#include "terminal.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

Terminal::Terminal(
  Shader shader,
  Shader text_shader
) : shader_(shader), text_shader_(text_shader) {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &element_buffer_);

  vertices_ = {
    { 0, WINDOW_HEIGHT, 0 },
    { 0, WINDOW_HEIGHT-300, 0 },
    { WINDOW_WIDTH, WINDOW_HEIGHT, 0 },
    { WINDOW_WIDTH, WINDOW_HEIGHT-300, 0 }
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

  LoadFonts();
  
  lines_.push_back("Terminal for Sybil 1.0");
  lines_.push_back("First version...");
  lines_.push_back("");
}

void Terminal::LoadFonts() {
  FT_Library ft;
  if (FT_Init_FreeType(&ft))
    cout << "ERROR::FREETYPE: Could not init FreeType Library" << endl;
  
  FT_Face face;
  if (FT_New_Face(ft, "fonts/ubuntu_monospace.ttf", 0, &face))
    cout << "ERROR::FREETYPE: Failed to load font" << endl; 

  if (FT_Set_Char_Size(face, 0, 16*64, 300, 300))
    cout << "ERROR::FREETYPE: Failed to set char size" << endl; 
   
  FT_Set_Pixel_Sizes(face, 0, 18);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  for (GLubyte c = 0; c < 128; c++) {
    // Load character glyph 
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        continue;
    }

    // Generate texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer
    );

    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Now store character for later use
    characters_[c] = {
      texture, 
      glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
      glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
      (GLuint) face->glyph->advance.x
    };
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
}

void Terminal::Draw(glm::vec3 position) {
  if (!enabled)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  stringstream ss;
  ss << "Position: " << position.x << " " << position.y << " " << position.z;
  lines_[2] = ss.str();

  int height = WINDOW_HEIGHT - 18;
  for (auto& l : lines_) {
    DrawText(l, 2, height);
    height -= 18;
  }

  glUseProgram(shader_.program_id());

  glm::mat4 projection = glm::ortho(0.0f, (float) WINDOW_WIDTH, 0.0f, (float) WINDOW_HEIGHT);
  glUniformMatrix4fv(shader_.GetUniformId("projection"), 1, GL_FALSE, &projection[0][0]);

  shader_.BindBuffer(vertex_buffer_, 0, 3);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*) 0);

  shader_.Clear();

  glDisable(GL_BLEND);
}

void Terminal::DrawText(const string& text, float x, float y, vec3 color) {
  glUseProgram(text_shader_.program_id());

  GLfloat scale = 1.0f;
  glm::mat4 projection = glm::ortho(0.0f, (float) WINDOW_WIDTH, 0.0f, (float) WINDOW_HEIGHT); 
  // Activate corresponding render state	
  glUniform3f(glGetUniformLocation(text_shader_.program_id(), "textColor"), color.x, color.y, color.z);
  glUniformMatrix4fv(text_shader_.GetUniformId("projection"), 1, GL_FALSE, &projection[0][0]);

  glActiveTexture(GL_TEXTURE0);

  // Iterate through all characters
  for (const auto& c : text) {
      Character ch = characters_[c];

      GLfloat xpos = x + ch.Bearing.x * scale;
      GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

      GLfloat w = ch.Size.x * scale;
      GLfloat h = ch.Size.y * scale;

      // Update VBO for each character
      GLfloat vertices[6][4] = {
        { xpos,     ypos + h,   0.0, 0.0 },            
        { xpos,     ypos,       0.0, 1.0 },
        { xpos + w, ypos,       1.0, 1.0 },

        { xpos,     ypos + h,   0.0, 0.0 },
        { xpos + w, ypos,       1.0, 1.0 },
        { xpos + w, ypos + h,   1.0, 0.0 }           
      };

      // Render glyph texture over quad
      glBindTexture(GL_TEXTURE_2D, ch.TextureID);

      // Update content of VBO memory
      text_shader_.BindBuffer(VBO, 0, 4);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

      // Render quad
      glDrawArrays(GL_TRIANGLES, 0, 6);

      // glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*) 0);
      // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
      x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
  }

  text_shader_.Clear();
}

} // End of namespace.
