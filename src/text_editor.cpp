#include "text_editor.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

string TextEditor::command = "";
bool TextEditor::on_g = false;
bool TextEditor::on_delete = false;
double TextEditor::repeat_wait = 0.0f;
bool TextEditor::ignore = false;
bool TextEditor::enabled = false;
double TextEditor::debounce_timer = 0.0f;
string TextEditor::write_buffer = "";
int TextEditor::cursor_row_ = 0;
int TextEditor::cursor_col_ = 0;
vector<string> TextEditor::content_ = {};
double TextEditor::cursor_debounce_timer_ = 0.0;
double TextEditor::cursor_timer = 0.0;
int TextEditor::mode = 0;
int TextEditor::start_line = 0;
string TextEditor::filename = "";

// Static function to process GLFW char input.
void TextEditor::PressCharCallback(GLFWwindow* window, unsigned char_code) {
  if (!enabled) return;
  if (char_code > 128) return;
  if (mode == 0) return;
  if (ignore) { 
    ignore = false;
    return;
  }

  switch (mode) {
    case 1:
      if (cursor_col_ == 0) {
        content_[cursor_row_] = ((char) char_code) + content_[cursor_row_];
      } else 
        content_[cursor_row_] = content_[cursor_row_].substr(0, cursor_col_) + ((char) char_code) + content_[cursor_row_].substr(cursor_col_);
      cursor_col_++;
      break;
    case 2:
      command += (char) char_code;
      break;
  }
}

void TextEditor::PressKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (!enabled)
    return;

  if (action == GLFW_REPEAT && mode == 0) {
    switch (key) {
      case GLFW_KEY_H: 
        if (mode == 1) break;
        if (cursor_col_ >= content_[cursor_row_].size()) cursor_col_ = content_[cursor_row_].size() - 1;
        if (cursor_col_ > 0) cursor_col_--;
        if (cursor_col_ < 0) cursor_col_ = 0;
        break;
      case GLFW_KEY_J: 
        if (mode == 1) break;
        if (cursor_row_ < content_.size() - 1) cursor_row_++;
        if (cursor_row_ >= start_line + 30) start_line++; 
        break;
      case GLFW_KEY_K: 
        if (mode == 1) break;
        if (cursor_row_ > 0) cursor_row_--;
        if (cursor_row_ < start_line) start_line--; 
        break;
      case GLFW_KEY_L: 
        if (mode == 1) break;
        if (content_.size() > 0 && cursor_col_ < content_[cursor_row_].size() - 1) cursor_col_++;
        break;
      case GLFW_KEY_X:
        if (cursor_col_ >= 0 && content_[cursor_row_].size()) {
          content_[cursor_row_] = content_[cursor_row_].substr(0, cursor_col_) + content_[cursor_row_].substr(cursor_col_+1);
        }
        break;
      default:
        break;
    }
  } else if (action == GLFW_REPEAT && mode == 1) {
    switch (key) {
      case GLFW_KEY_BACKSPACE: {
        if (cursor_col_ == 0 && cursor_row_ > 0) {
          content_.erase(content_.begin() + cursor_row_);
          cursor_row_--;
          if (cursor_row_ < start_line) start_line--;
          cursor_col_ = content_[cursor_row_].size();
        } else if (cursor_row_ > 0) {
          content_[cursor_row_] = content_[cursor_row_].substr(0, cursor_col_-1) + content_[cursor_row_].substr(cursor_col_);
          cursor_col_--;
        }
        break;
      }
      case GLFW_KEY_ENTER:
        string before = content_[cursor_row_].substr(0, cursor_col_);
        vector<string> after { content_[cursor_row_].substr(cursor_col_) };
        content_[cursor_row_] = before;
        content_.insert(content_.begin() + cursor_row_ + 1, after.begin(), after.end());
        cursor_row_++;
        if (cursor_row_ >= start_line + 30) start_line++;
        cursor_col_ = 0;
        break;
    }
  } else if (action != GLFW_PRESS) {
    return;
  }

  if (mode == 0) {
    switch (key) {
      case GLFW_KEY_G: {
        if (mods & GLFW_MOD_SHIFT) {
          start_line = std::max(0, (int) content_.size() - 30);
          cursor_row_ = content_.size() - 1;
          cursor_col_ = content_[content_.size()-1].size() - 1;
          break;
        }

        if (on_g) {
          start_line = 0;
          cursor_row_ = 0;
          cursor_col_ = 0;
          on_g = false;
        } else {
          on_g = true;
        }
        break;
      }
      case GLFW_KEY_H: 
        if (mode == 1) break;
        if (cursor_col_ >= content_[cursor_row_].size()) cursor_col_ = content_[cursor_row_].size() - 1;
        if (cursor_col_ > 0) cursor_col_--;
        if (cursor_col_ < 0) cursor_col_ = 0;
        repeat_wait = glfwGetTime() + 0.5;
        break;
      case GLFW_KEY_J: 
        if (mode == 1) break;
        if (cursor_row_ < content_.size() - 1) cursor_row_++;
        if (cursor_row_ >= start_line + 30) start_line++; 
        repeat_wait = glfwGetTime() + 0.5;
        break;
      case GLFW_KEY_K: 
        if (mode == 1) break;
        if (cursor_row_ > 0) cursor_row_--;
        if (cursor_row_ < start_line) start_line--; 
        repeat_wait = glfwGetTime() + 0.5;
        break;
      case GLFW_KEY_L: 
        if (mode == 1) break;
        if (content_.size() > 0 && cursor_col_ < content_[cursor_row_].size() - 1) cursor_col_++;
        repeat_wait = glfwGetTime() + 0.5;
        break;
      case GLFW_KEY_I: {
        if (mods & GLFW_MOD_SHIFT) {
          cursor_col_ = 0;
          if (content_.size() > 0) {
            while (cursor_col_ < content_[cursor_row_].size() && content_[cursor_row_][cursor_col_] == ' ') 
              cursor_col_++;
          }
 
          ignore = true;
          mode = 1;
        } else {
          if (content_.size() > 0 && cursor_col_ >= content_[cursor_row_].size()) cursor_col_ = content_[cursor_row_].size() - 1;
          if (cursor_col_ < 0) cursor_col_ = 0;
          ignore = true;
          mode = 1;
        }
        break;
      }
      case GLFW_KEY_X:
        if (cursor_col_ >= 0 && content_[cursor_row_].size()) {
          content_[cursor_row_] = content_[cursor_row_].substr(0, cursor_col_) + content_[cursor_row_].substr(cursor_col_+1);
        }
        break;
      case GLFW_KEY_S:
        if (cursor_col_ >= 0 && content_[cursor_row_].size()) {
          content_[cursor_row_] = content_[cursor_row_].substr(0, cursor_col_) + content_[cursor_row_].substr(cursor_col_+1);
        }
        if (content_.size() > 0 && cursor_col_ >= content_[cursor_row_].size()) cursor_col_ = content_[cursor_row_].size() - 1;
        if (cursor_col_ < 0) cursor_col_ = 0;
        ignore = true;
        mode = 1;
        break;
      case GLFW_KEY_A:
        if (mods & GLFW_MOD_SHIFT) {
          if (content_.size() > 0) cursor_col_ = content_[cursor_row_].size();
          ignore = true;
          mode = 1;
        } else {
          cursor_col_++;
          if (content_.size() > 0 && cursor_col_ >= content_[cursor_row_].size()) cursor_col_ = content_[cursor_row_].size() - 1;
          if (cursor_col_ < 0) cursor_col_ = 0;
          ignore = true;
          mode = 1;
        }
        break;
      case GLFW_KEY_O:
        if (mods & GLFW_MOD_SHIFT) {
          vector<string> after { "" };
          content_.insert(content_.begin() + cursor_row_, after.begin(), after.end());
          cursor_col_ = 0;
          ignore = true;
          mode = 1;
        } else {
          vector<string> after { "" };
          content_.insert(content_.begin() + cursor_row_ + 1, after.begin(), after.end());
          cursor_row_++;
          if (cursor_row_ >= start_line + 30) start_line++;
          cursor_col_ = 0;
          ignore = true;
          mode = 1;
        }
        break;
      case GLFW_KEY_U: {
        if (mods & GLFW_MOD_CONTROL) {
          start_line = std::max(start_line - 10, 0);
          cursor_row_ = std::max(cursor_row_ - 10, 0);
        }
        break;
      }
      case GLFW_KEY_D: {
        if (mods & GLFW_MOD_CONTROL) {
          if (start_line >= (int) content_.size() - 30) {
            cursor_row_ = content_.size() - 1;
            break;
          }
          start_line = std::min(start_line + 10, (int) content_.size() - 1);
          cursor_row_ = std::min(cursor_row_ + 10, (int) content_.size() - 1);
          break;
        }

        if (on_delete) {
          content_.erase(content_.begin() + cursor_row_);
          if (content_.size() == 0)
            content_.push_back("");
          if (cursor_row_ >= content_.size()) 
            cursor_row_--;
          on_delete = false;
          cursor_col_ = 0;
        } else {
          on_delete = true;
        }
        break;
      }  
      case GLFW_KEY_ESCAPE: 
        on_delete = false;
        on_g = false;
        break;
      case GLFW_KEY_SEMICOLON:
      case GLFW_KEY_SLASH: // For Portuguese keyboards.
        if (mods & GLFW_MOD_SHIFT) {
          mode = 2;
          command = "";
        }
        break;
      default:
        return;
    }
  }

  if (mode == 1) {
    switch (key) {
      case GLFW_KEY_ESCAPE: 
        mode = 0;
        on_delete = false;
        break;
      case GLFW_KEY_BACKSPACE:
        if (cursor_col_ == 0 && cursor_row_ > 0) {
          content_.erase(content_.begin() + cursor_row_);
          cursor_row_--;
          if (cursor_row_ < start_line) start_line--;
          cursor_col_ = content_[cursor_row_].size();
        } else if (cursor_col_ > 0) {
          content_[cursor_row_] = content_[cursor_row_].substr(0, cursor_col_-1) + content_[cursor_row_].substr(cursor_col_);
          cursor_col_--;
        }
        break;
      case GLFW_KEY_ENTER:
        string before = content_[cursor_row_].substr(0, cursor_col_);
        vector<string> after { content_[cursor_row_].substr(cursor_col_) };
        content_[cursor_row_] = before;
        content_.insert(content_.begin() + cursor_row_ + 1, after.begin(), after.end());
        cursor_row_++;
        if (cursor_row_ >= start_line + 30) start_line++;
        cursor_col_ = 0;
        break;
    }
  }

  if (mode == 2) {
    switch (key) {
      case GLFW_KEY_BACKSPACE: {
        if (command.size() == 1) {
          mode = 0;
        } else {
          command = command.substr(0, command.size()-1);
        }
        break;
      }
      case GLFW_KEY_ENTER: {
        mode = 0;
        string cmd = command.substr(1);
        if (cmd == "w") {
          WriteFile();
        } else if (cmd == "q") {
          enabled = false; 
        } else if (cmd == "wq") {
          WriteFile();
          enabled = false; 
        }
        break;
      }
      case GLFW_KEY_ESCAPE: {
        mode = 0;
        break;
      }
    }
  }

  cursor_timer = (glfwGetTime() + 1.0);
}

void TextEditor::OpenFile(string new_filename) {
  filename = new_filename;
  ifstream f(filename);
  if (!f.is_open()) return;
  string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
  TextEditor::SetContent(content);
}

void TextEditor::WriteFile() {
  ofstream f(filename, std::ofstream::out);
  if (!f.is_open()) return;

  for (int i = 0; i < content_.size(); i++) {
    f << content_[i] << endl; 
  }
  f.close();
}

void TextEditor::SetContent(string text) {
  boost::split(content_, text, boost::is_any_of("\n"));

  if (content_.size() == 0)
    content_.push_back("");
}

TextEditor& TextEditor::GetInstance() {
  static TextEditor instance; 
  return instance;
}

void TextEditor::Draw() {
  if (!enabled)
    return;

  Graphics::GetInstance().Rectangle(199, WINDOW_HEIGHT - 99, 802, 602, vec3(1, 0.69, 0.23));
  Graphics::GetInstance().Rectangle(200, WINDOW_HEIGHT - 100, 800, 600, vec3(0.3));

  double current_time = glfwGetTime();
  if (current_time > cursor_timer)
    cursor_timer = current_time + 1.0;

  vector<string> lines = content_;

  int digits = 0;
  for (int aux = lines.size(); aux; aux /= 10) { digits++; }
  digits = std::max(digits, 3);

  int base_x = 200 + digits * 9 + 8;
  int base_y = WINDOW_HEIGHT - 100 - LINE_HEIGHT;
  int height = 0;
  for (int y = start_line; y < start_line + 30; ++y) {
    if (y >= lines.size()) break;
    stringstream ss;
    ss << y + 1;

    int digits_ = 0;
    for (int aux = y + 1; aux; aux /= 10) { digits_++; }
    int offset_x = (digits - digits_) * 9;
    Graphics::GetInstance().DrawText(ss.str(), 200 + 2 + offset_x, base_y - height, vec3(1, 0.69, 0.23));

    if (lines[y].size() == 0 && cursor_row_ == y && mode != 2) {
      Graphics::GetInstance().DrawChar((char) 150, base_x + 2, base_y - height);
    }

    for (int x = 0; x < 80; ++x) {
      if (x >= lines[y].size()) break;

      bool draw_cursor = false;
      if (cursor_col_ == x && cursor_row_ == y) draw_cursor = true;
      if (cursor_row_ == y && cursor_col_ >= lines[y].size() && x == lines[y].size() - 1 && mode == 0) draw_cursor = true;
      if (mode == 2) draw_cursor = false;

      if (draw_cursor) {
        Graphics::GetInstance().DrawChar((char) 150, base_x + 2 + x * 9, base_y - height);
        Graphics::GetInstance().DrawChar(lines[y][x], base_x + 2 + x * 9, base_y - height, vec3(0, 0, 0));
      } else {
        Graphics::GetInstance().DrawChar(lines[y][x], base_x + 2 + x * 9, base_y - height);
      }
    }

    if (cursor_col_ == lines[y].size() && cursor_row_ == y && mode == 1) {
      Graphics::GetInstance().DrawChar((char) 150, base_x + 2 + lines[y].size() * 9, base_y - height);
    }
    height += LINE_HEIGHT;
  }

  for (int y = lines.size(); y < 30; ++y) {
    Graphics::GetInstance().DrawText("~", 200 + 2, base_y - height, vec3(1, 0.69, 0.23));
    height += LINE_HEIGHT;
  }

  Graphics::GetInstance().Rectangle(200 + 2, base_y - LINE_HEIGHT * 29 - 3, 796, LINE_HEIGHT, vec3(1, 0.69, 0.23));
  Graphics::GetInstance().DrawText(filename, 200 + 2, base_y - LINE_HEIGHT * 30, vec3(0.3));

  if (mode == 1) {
    Graphics::GetInstance().DrawText("-- INSERT --", 200 + 2, base_y - LINE_HEIGHT * 32, vec3(1, 0.69, 0.23));
  }

  // Cursor.
  if (mode == 2) {
    Graphics::GetInstance().DrawText(command, 200 + 2, base_y - LINE_HEIGHT * 31, vec3(1, 0.69, 0.23));
    Graphics::GetInstance().DrawChar((char) 150, 200 + 2 + command.size() * 9, base_y - LINE_HEIGHT * 31, vec3(1, 0.69, 0.23));
  }
}

} // End of namespace.
