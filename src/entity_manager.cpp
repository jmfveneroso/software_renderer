#include "entity_manager.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

EntityManager::EntityManager(
  shared_ptr<GameState> game_state,
  shared_ptr<Renderer> renderer,
  shared_ptr<TextEditor> text_editor,
  shared_ptr<Building> building
) : game_state_(game_state), 
    renderer_(renderer), 
    text_editor_(text_editor), 
    building_(building) {
  Init();
}

void EntityManager::Init() {
  scrolls_.push_back(Scroll(id_counter++, vec3(2009.25, 206, 2000.5), radians(-90.0f), "files/scroll_1.txt"));
  scrolls_.push_back(Scroll(id_counter++, vec3(2009.25, 206, 2001.5), radians(-90.0f), "files/scroll_2.txt"));
  scrolls_.push_back(Scroll(id_counter++, vec3(2009.25, 206, 2002.5), radians(-90.0f), "files/scroll_3.txt"));
  scrolls_.push_back(Scroll(id_counter++, vec3(2009.25, 206, 2003.5), radians(-90.0f), "files/scroll_4.txt"));
  scrolls_.push_back(Scroll(id_counter++, vec3(2009.25, 206, 2004.5), radians(-90.0f), "files/scroll_5.txt"));

  objects_.push_back(Object(id_counter++, vec3(2009, 205, 1996), radians(-90.0f), "book_stand"));

  plots_.push_back(CreatePlot("files/plot1.txt", vec3(1996.5, 206.5, 1995.05), 0.0f));
  plots_.push_back(CreatePlot("files/plot2.txt", vec3(1995.05, 206.5, 1997), radians(90.0f)));
  plots_.push_back(CreatePlot("files/plot3.txt", vec3(1995.05, 206.5, 2000), radians(90.0f)));
  plots_.push_back(CreatePlot("files/plot3.txt", vec3(1995.05, 206.5, 2000), 0.0f));
}

vec3 EntityManager::GetColor(const string& color_name) {
  unordered_map<string, vec3> colors {
    { "red",  vec3(1, 0, 0) },
    { "green",  vec3(0, 1, 0) },
    { "blue",  vec3(0, 0, 1) },
    { "yellow",  vec3(1, 1, 0) },
    { "magenta",  vec3(1, 0, 1) },
    { "white",  vec3(1, 1, 1) },
    { "black",  vec3(0, 0, 0) }
  };
  return colors[color_name];
}

Plot EntityManager::CreatePlot(const string& filename, vec3 position, GLfloat rotation) {
  Plot p = Plot(id_counter++, filename, position, rotation);
  renderer_->CreateFramebuffer(filename, 1024, 1024);
  UpdatePlot(p);
  return p;
}

void EntityManager::UpdatePlot(Plot& plot) {
  ifstream f(plot.filename);
  if (!f.is_open()) return;
  string content((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());

  vector<string> lines;
  boost::split(lines, content, boost::is_any_of("\n"));

  renderer_->SetFBO(plot.filename);
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

      color = GetColor(tkns[6]);
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
      color = GetColor(tkns[4]);

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
      color = GetColor(tkns[4]);

      point *= pixels_per_step_;
      renderer_->DrawText(text, point.x, point.y, color);
    }
  }

  renderer_->set_projection();
}

void EntityManager::Update() {
  mat4 ProjectionMatrix = game_state_->projection_matrix();
  mat4 ViewMatrix = game_state_->view_matrix();

  if (text_editor_->update_object) {
    for (auto& p : plots_) {
      if (p.id == active_object_id_) {
        UpdatePlot(p); 
        break;
      }
    }
    active_object_id_ = 0;
    text_editor_->update_object = false;
  }

  for (auto& s : scrolls_) s.highlighted = false;
  for (auto& p : plots_) p.highlighted = false;

  GLfloat distance = numeric_limits<GLfloat>::max();
  Object* selected_obj = nullptr;

  vector<Object*> objs;
  for (auto& s : scrolls_) objs.push_back(&s);
  for (auto& p : plots_) objs.push_back(&p);

  for (auto& o : objs) {
    glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), o->position_);
    ModelMatrix *= glm::rotate(glm::mat4(1.0f), o->rotation_, glm::vec3(0.0, 1.0, 0.0));
    vec4 screen_coords = (ProjectionMatrix * ViewMatrix * ModelMatrix) * vec4(0, 0, 0, 1);
    if (screen_coords.z < 0) continue;

    GLfloat cur_distance = length2(vec2(screen_coords.x, screen_coords.y));
    if (cur_distance > 4) continue;

    if (cur_distance < distance) {
      distance = cur_distance;
      selected_obj = o;
    }
  }

  if (selected_obj) {
    selected_obj->highlighted = true;
  }
}

void EntityManager::Draw() {
  mat4 ProjectionMatrix = game_state_->projection_matrix();
  mat4 ViewMatrix = game_state_->view_matrix();
  vec3 camera = game_state_->camera().position;

  building_->Draw(ProjectionMatrix, ViewMatrix, camera);

  for (auto& s : scrolls_) {
    renderer_->DrawMesh(s.mesh_name_, ProjectionMatrix, ViewMatrix, camera, s.position_, s.rotation_, s.highlighted);
  }

  for (auto& o : objects_) {
    renderer_->DrawMesh(o.mesh_name_, ProjectionMatrix, ViewMatrix, camera, o.position_, o.rotation_, false);
  }

  for (int i = 0; i < 3; i++) {
    auto& p = plots_[i];
    FBO fbo = renderer_->GetFBO(p.filename);
    renderer_->DrawHighlightedObject(
      "2d_plot", ProjectionMatrix, ViewMatrix, camera, 
      p.position_, p.rotation_, p.highlighted, fbo.texture
    );
  }

  // Collide plot.
  vec3 player_pos = game_state_->player().position;
  vec3 direction = game_state_->camera().direction;

  PointIntersection intersection = building_->GetPointIntersection(player_pos, direction);

  GLfloat rotation = glm::radians(int(4 * game_state_->player().h_angle / (PI * 2)) * 90.0f);
  GLfloat distance = 10.0f;
  bool collision = false;
  if (intersection.distance < 10.0f) {
    distance = intersection.distance;
    if (intersection.normal.z > 0)
      rotation = glm::radians(0.0f);
    else if (intersection.normal.x > 0) 
      rotation = glm::radians(-90.0f);
    else if (intersection.normal.z < 0) 
      rotation = glm::radians(-180.0f);
    else if (intersection.normal.x < 0) 
      rotation = glm::radians(-270.0f);
    collision = true;
  }

  auto& p = plots_[3];
  vec3 next_pos = player_pos + direction * distance;
  
  if (collision)
    next_pos += intersection.normal * 0.05f;

  if (terrain_) {
    float height = terrain_->GetHeight(next_pos.x, next_pos.z);
    if (next_pos.y - 1.0f < height) {
      next_pos.y = height + 1.0f;
    }
  }

  FBO fbo = renderer_->GetFBO(p.filename);
  renderer_->DrawHighlightedObject(
    "2d_plot", ProjectionMatrix, ViewMatrix, camera, 
    next_pos, rotation, collision, fbo.texture, 0.8
  );
}

void EntityManager::Interact() {
  for (auto& s : scrolls_) {
    if (s.highlighted) {
      text_editor_->Enable();
      text_editor_->OpenFile(s.filename);
      game_state_->ChangeMode(TXT);
      active_object_id_ = s.id;
      break;
    }
  }

  for (auto& p : plots_) {
    if (p.highlighted) {
      UpdatePlot(p);
      text_editor_->Enable();
      text_editor_->OpenFile(p.filename);
      game_state_->ChangeMode(TXT);
      active_object_id_ = p.id;
      text_editor_->update_object = false;
      break;
    }
  }
}

void EntityManager::Collide(glm::vec3& player_pos, glm::vec3 prev_pos, bool& can_jump, glm::vec3& speed) {
  // Player bounding box.
  BoundingBox p = BoundingBox(
    player_pos.x - 0.35, 
    player_pos.y - 1.5, 
    player_pos.z - 0.35, 
    0.7, 1.5, 0.7
  );

  building_->Collide(player_pos, prev_pos, can_jump, speed, p);
}

} // End of namespace.
