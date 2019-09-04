#include "terminal.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

bool Terminal::enabled = false;
double Terminal::debounce_timer = 0.0f;
string Terminal::write_buffer = "";

// Static function to process GLFW char input.
void Terminal::PressKey(GLFWwindow* window, unsigned char_code) {
  if (!enabled) return;
  if (char_code > 128) return;
  write_buffer += (char) char_code;
}

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
  lines_.push_back("Player position...");
  NewLine(true);
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

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  cursor_character_ = characters_[150];
}

void Terminal::Backspace() {
  if (lines_.back().size() <= 2) return;
  lines_.back() = lines_.back().substr(0, lines_.back().size()-1);
}

void Terminal::NewLine(bool dynamic_text) {
  if (dynamic_text)
    lines_.push_back("$ ");
  else
    lines_.push_back("");
}

bool Terminal::Move(Player& player, vector<string>& tokens) {
  if (tokens.size() < 4) {
    lines_.push_back(string("Invalid number of arguments."));
    return false;
  }

  glm::vec3 new_position;
  new_position.x = boost::lexical_cast<float>(tokens[1]); 
  new_position.y = boost::lexical_cast<float>(tokens[2]);
  new_position.z = boost::lexical_cast<float>(tokens[3]);
  player.position = new_position;
  lines_.push_back(string("Moved player to a new position."));
  return true;
}

void Terminal::Execute(GameState& game_state, Player& player) {
  if (lines_.size() <= 2) {
    NewLine(true);
    return;
  }

  string line = lines_.back().substr(2);
  if (line.size() == 0) {
    NewLine(true);
    return; 
  }

  vector<string> tokens;
  boost::split(tokens, line, boost::is_any_of(" "));

  string command = tokens[0];
  if (command == "clear") {
    Clear();
  } else if (command == "exit") {
    enabled = false;
    game_state = FREE;
  } else if (command == "move") {
    Move(player, tokens);
  } else {
    lines_.push_back(string("Invalid command: ") + command);
  }

  history_.push_back(line);
  NewLine(true);
}

void Terminal::Clear() {
  lines_ = vector<string>(lines_.begin(), lines_.begin() + 2);
}

void Terminal::Update() {
  if (write_buffer.size() > 0) {
    lines_.back() += write_buffer;
    write_buffer = "";
  }
}

void Terminal::Draw(glm::vec3 position) {
  if (!enabled)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  stringstream ss;
  ss << "Position: " << position.x << " " << position.y << " " << position.z;
  lines_[1] = ss.str();

  int height = WINDOW_HEIGHT - LINE_HEIGHT;
  for (int i = 0; i < lines_.size(); ++i) {
    bool draw_cursor = i == (lines_.size() - 1);

    double current_time = glfwGetTime();
    if (current_time - (int) current_time > 0.5)
      draw_cursor = false;
    
    DrawText(lines_[i], 2, height, draw_cursor);
    height -= LINE_HEIGHT;
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

void Terminal::DrawChar(Character& ch, float x, float y, vec3 color) {
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
  text_shader_.BindBuffer(VBO, 0, 4);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

  // Render quad
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Terminal::DrawText(const string& text, float x, float y, bool draw_cursor, vec3 color) {
  glUseProgram(text_shader_.program_id());

  glm::mat4 projection = glm::ortho(0.0f, (float) WINDOW_WIDTH, 0.0f, (float) WINDOW_HEIGHT); 

  // Activate corresponding render state	
  glUniform3f(glGetUniformLocation(text_shader_.program_id(), "textColor"), color.x, color.y, color.z);
  glUniformMatrix4fv(text_shader_.GetUniformId("projection"), 1, GL_FALSE, &projection[0][0]);

  glActiveTexture(GL_TEXTURE0);

  // Iterate through all characters
  for (const auto& c : text) {
    DrawChar(characters_[c], x, y, color);

    // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    float scale = 1.0;
    x += (characters_[c].Advance >> 6) * scale;
  }

  if (draw_cursor) {
    DrawChar(cursor_character_, x, y, color);
  }

  text_shader_.Clear();
}

} // End of namespace.
