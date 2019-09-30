#include "plotter.hpp"

using namespace std;
using namespace glm;
using namespace boost::filesystem;

namespace Sibyl {

Plotter::Plotter(
  shared_ptr<GameState> game_state,
  shared_ptr<Renderer> renderer,
  shared_ptr<TextEditor> text_editor
) : game_state_(game_state),
    renderer_(renderer),
    text_editor_(text_editor) {
}

void Plotter::UpdatePlot(const string& filename, const string& fbo_name) {
  std::ifstream f(filename);
  if (!f.is_open()) return;
  string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());

  vector<string> lines;
  boost::split(lines, content, boost::is_any_of("\n"));

  renderer_->SetFBO(fbo_name);
  renderer_->Clear(1.0, 1.0, 1.0);
  vec2 texture_size_(512, 512);
  renderer_->set_projection(glm::ortho(-texture_size_.x/2, texture_size_.x/2, texture_size_.y/2, -texture_size_.y/2));

  boost::regex re("(\"[^\"]+\")|[#A-Za-z0-9.-]+");
  for (int i = 0; i < lines.size(); i++) {
    boost::sregex_token_iterator j(lines[i].begin(), lines[i].end(), re, 0);
    boost::sregex_token_iterator k;
 
    vector<string> tkns;
    while (j != k) {
      tkns.push_back(*j++);
    }

    if (tkns.size() == 0) continue;

    int step = (400 / 20);
    GLfloat pixels_per_step_ = step;

    string command = tkns[0];
    if (command == "#") continue;

    if (command == "XAxis") {
      int max_value = 10;
      int tick_step = 1;
      int big_tick_step = 5;
     
      max_value = boost::lexical_cast<int>(tkns[1]); 
      if (tkns.size() > 2) {
        tick_step = boost::lexical_cast<int>(tkns[2]);
        big_tick_step = boost::lexical_cast<int>(tkns[3]);
      } 

      DrawXAxis(max_value, tick_step, big_tick_step);
    }

    if (command == "YAxis") {
      int max_value = 10;
      int tick_step = 1;
      int big_tick_step = 5;
     
      max_value = boost::lexical_cast<int>(tkns[1]); 
      if (tkns.size() > 2) {
        tick_step = boost::lexical_cast<int>(tkns[2]);
        big_tick_step = boost::lexical_cast<int>(tkns[3]);
      } 

      DrawYAxis(max_value, tick_step, big_tick_step);
    }

    if (command == "Grid") {
      int num_tiles = boost::lexical_cast<int>(tkns[1]); 
      DrawGrid(num_tiles);
    }

    if (command == "Arrow" || command == "Line") {
      int size = 440;

      vec2 p1(0, 0); 
      vec2 p2(0, 0);
      GLfloat thickness = 1;
      vec3 color(0, 0, 0);

      if (tkns.size() == 4) {
        p2.x = boost::lexical_cast<float>(tkns[1]); 
        p2.y = boost::lexical_cast<float>(tkns[2]);
        color = renderer_->GetColor(tkns[3]);
      } else {
        p1.x = boost::lexical_cast<float>(tkns[1]); 
        p1.y = boost::lexical_cast<float>(tkns[2]);
        p2.x = boost::lexical_cast<float>(tkns[3]); 
        p2.y = boost::lexical_cast<float>(tkns[4]);
        color = renderer_->GetColor(tkns[5]);
      }

      // thickness = boost::lexical_cast<float>(tkns[5]);

      p1 *= size / 20;
      p2 *= size / 20;

      if (command == "Arrow") {
        renderer_->DrawArrow(p1, p2, thickness, color);
      } else {
        renderer_->DrawLine(p1, p2, thickness, color);
      }
    }

    if (command == "Point") {
      vec2 point; 
      GLfloat thickness;
      vec3 color;

      point.x = boost::lexical_cast<float>(tkns[1]); 
      point.y = boost::lexical_cast<float>(tkns[2]);
      thickness = boost::lexical_cast<float>(tkns[3]);
      color = renderer_->GetColor(tkns[4]);

      point *= pixels_per_step_;
      renderer_->DrawPoint(point, thickness, color);
    }

    if (command == "Text") {
      string text;
      vec2 point; 
      vec3 color;

      text = tkns[1].substr(1, tkns[1].size() - 2);
      point.x = boost::lexical_cast<float>(tkns[2]); 
      point.y = boost::lexical_cast<float>(tkns[3]);
      color = renderer_->GetColor(tkns[4]);
     
      float scale = 1.0;
      if (tkns.size() == 6) 
        scale = boost::lexical_cast<float>(tkns[5]);

      point *= pixels_per_step_;
      renderer_->DrawText(text, point.x, point.y, color, scale);
    }
  }

  renderer_->SetFBO("screen");
  renderer_->set_projection();
}

void Plotter::DrawGrid(int num_tiles) {
  int size = 440;
  int padding = (512 - size) / 2;
  int start = -256 + padding;

  for (int x = 0; x <= num_tiles; x++) {
    int cur_x = start + (x * size) / num_tiles;
    renderer_->DrawLine(vec2(-size/2, cur_x), vec2(size/2, cur_x), 1, vec3(0.8));
    renderer_->DrawLine(vec2(cur_x, -size/2), vec2(cur_x, size/2), 1, vec3(0.8));
  }
}

void Plotter::DrawXAxis(
  int max_value, int tick_step, int big_tick_step
) {
  int size = 440 / 2;

  renderer_->DrawLine(vec2(0, -9), vec2(0, 9), 1, vec3(0));
  renderer_->DrawLine(vec2(-size, 0), vec2(size, 0), 1, vec3(0));

  // Small ticks.
  for (int x = -max_value; x <= max_value; x += tick_step) {
    int cur_x = (x * size) / max_value;
    renderer_->DrawLine(vec2(cur_x, -2), vec2(cur_x, 2), 1, vec3(0));
  }

  // Big ticks.
  for (int x = -max_value; x <= max_value; x += big_tick_step) {
    if (x == 0) continue;

    int cur_x = (x * size) / max_value;
    renderer_->DrawLine(vec2(cur_x, -4), vec2(cur_x, 4), 1, vec3(0));

    stringstream ss;
    ss << x;
    renderer_->DrawText(ss.str(), cur_x, -30, vec3(0), 1, true);
  }
}

void Plotter::DrawYAxis(
  int max_value, int tick_step, int big_tick_step
) {
  int size = 440 / 2;

  renderer_->DrawLine(vec2(0, -9), vec2(0, 9), 1, vec3(0));
  renderer_->DrawLine(vec2(1, -size), vec2(1, size), 1, vec3(0));

  // Small ticks.
  for (int y = -max_value; y <= max_value; y += tick_step) {
    int cur_y = (y * size) / max_value;
    renderer_->DrawLine(vec2(-2, cur_y), vec2(2, cur_y), 1, vec3(0));
  }

  // Big ticks.
  for (int y = -max_value; y <= max_value; y += big_tick_step) {
    if (y == 0) continue;

    int cur_y = (y * size) / max_value;
    renderer_->DrawLine(vec2(-4, cur_y), vec2(4, cur_y), 1, vec3(0));

    stringstream ss;
    ss << y;
    renderer_->DrawText(ss.str(), -30, cur_y - 5, vec3(0), 1, false);
  }
}

} // End of namespace.
