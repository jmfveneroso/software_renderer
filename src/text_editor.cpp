#include "text_editor.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

bool TextEditor::enabled = false;
double TextEditor::debounce_timer = 0.0f;
string TextEditor::write_buffer = "";

// Static function to process GLFW char input.
void TextEditor::PressKey(GLFWwindow* window, unsigned char_code) {
  if (!enabled) return;
  if (char_code > 128) return;
  write_buffer += (char) char_code;
}

TextEditor& TextEditor::GetInstance() {
  static TextEditor instance; 
  return instance;
}

bool TextEditor::Enable(GameState& state, bool set_enabled) { 
  double current_time = glfwGetTime();
  if (current_time <= debounce_timer) {
    debounce_timer = current_time + DEBOUNCE_DELAY; 
    return false;
  }

  enabled = set_enabled;
  if (enabled) 
    state = TXT; 
  else
    state = FREE;
  debounce_timer = current_time + DEBOUNCE_DELAY; 
  return true;
}

void TextEditor::MoveCursor(int dir_x, int dir_y) {
  double current_time = glfwGetTime();
  if (current_time <= cursor_debounce_timer_) {
    cursor_debounce_timer_ = current_time + 0.05; 
    return;
  }

  draw_cursor = (current_time + 1.0);
  cursor_row_ += dir_y;
  cursor_col_ += dir_x;
  cursor_debounce_timer_ = current_time + 0.05; 
}

void TextEditor::Draw() {
  if (!enabled)
    return;

  // // Update content.
  // if (write_buffer.size() > 0) {
  //   // lines_.back() += write_buffer;
  //   write_buffer = "";
  // }

  Graphics::GetInstance().Rectangle(50, WINDOW_HEIGHT - 50, WINDOW_WIDTH - 100, 300, vec3(0, 1, 0));

  double current_time = glfwGetTime();
  vector<string> lines;
  boost::split(lines, content_, boost::is_any_of("\n"));

  int base_x = 50;
  int base_y = WINDOW_HEIGHT - 50 - LINE_HEIGHT;
  int height = 0;
  for (int y = 0; y < lines.size(); ++y) {
    for (int x = 0; x < 80; ++x) {
      if (x >= lines[y].size()) break;

      if (current_time < draw_cursor - 0.5 && cursor_row_ == y && cursor_col_ == x) {
        Graphics::GetInstance().DrawChar((char) 150, base_x + 2 + x * 9, base_y - height);
        Graphics::GetInstance().DrawChar(lines[y][x], base_x + 2 + x * 9, base_y - height, vec3(0, 0, 0));
      } else {
        if (current_time > draw_cursor)
          draw_cursor = current_time + 1.0;
        Graphics::GetInstance().DrawChar(lines[y][x], base_x + 2 + x * 9, base_y - height);
      }
    }
    height += LINE_HEIGHT;
  }
}

} // End of namespace.
