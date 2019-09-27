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
  vec2 texture_size_(1024, 1024);
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
    if (command == "2D" || command == "1D") {
      int max_value;
      int tick_step;
      int big_tick_step;
      
      max_value = boost::lexical_cast<int>(tkns[1]); 
      tick_step = boost::lexical_cast<int>(tkns[2]);
      big_tick_step = boost::lexical_cast<int>(tkns[3]);

      if (command == "2D") {
        renderer_->DrawCartesianGrid(max_value, tick_step, big_tick_step);
      } else {
        renderer_->DrawOneDimensionalSpace(max_value, tick_step, big_tick_step);
      }
    }

    if (command == "Arrow" || command == "Line") {
      vec2 p1; 
      vec2 p2;
      GLfloat thickness;
      vec3 color;

      p1.x = boost::lexical_cast<float>(tkns[1]); 
      p1.y = boost::lexical_cast<float>(tkns[2]);

      p2.x = boost::lexical_cast<float>(tkns[3]); 
      p2.y = boost::lexical_cast<float>(tkns[4]);

      thickness = boost::lexical_cast<float>(tkns[5]);

      p1 *= pixels_per_step_;
      p2 *= pixels_per_step_;

      color = renderer_->GetColor(tkns[6]);
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

  renderer_->set_projection();
}

} // End of namespace.
