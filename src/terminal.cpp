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

Terminal::Terminal() 
  : shader_("terminal") {
  lines_.push_back("Terminal for Sybil 1.0");
  lines_.push_back("Player position...");
  NewLine(true);
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
   
    if (draw_cursor) 
      Graphics::GetInstance().DrawText(lines_[i] + ((char) 150), 2, height);
    else
      Graphics::GetInstance().DrawText(lines_[i], 2, height);
    height -= LINE_HEIGHT;
  }

  Graphics::GetInstance().Rectangle(0.0, WINDOW_HEIGHT, WINDOW_WIDTH, 300, vec3(0, 1, 0));
  glDisable(GL_BLEND);
}

} // End of namespace.
