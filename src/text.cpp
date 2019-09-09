#include "text.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {
Text::Text() : shader_("text") {
  LoadFonts();
}

Text& Text::GetInstance() {
  static Text instance; 
  return instance;
}

void Text::LoadFonts() {
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
  for (GLubyte c = 0; c < 255; c++) {
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

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

  SetProjection((float) WINDOW_WIDTH, (float) WINDOW_HEIGHT); 
}

void Text::DrawChar(char c, float x, float y, vec3 color) {
  Character& ch = characters_[c];

  GLfloat scale = 1.0f;
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
  shader_.BindBuffer(vbo_, 0, 4);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

  // Render quad
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Text::SetProjection(GLfloat width, GLfloat height) {
  projection_ = glm::ortho(0.0f, width, 0.0f, height); 
}

void Text::DrawText(const string& text, float x, float y, vec3 color) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(shader_.program_id());

  // Activate corresponding render state	
  glUniform3f(glGetUniformLocation(shader_.program_id(), "textColor"), color.x, color.y, color.z);
  glUniformMatrix4fv(shader_.GetUniformId("projection"), 1, GL_FALSE, &projection_[0][0]);
  glActiveTexture(GL_TEXTURE0);

  // Iterate through all characters
  for (const auto& c : text) {
    DrawChar(c, x, y, color);

    // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    float scale = 1.0;
    x += (characters_[c].Advance >> 6) * scale;
  }

  shader_.Clear();
  glDisable(GL_BLEND);
}

} // End of namespace.
