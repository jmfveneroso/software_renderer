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

void EntityManager::CollideWithFloor(Floor& f, vec3& pos, BoundingBox& p) {
  // AABB collision.
  if (
      p.x >= f.position.x + f.width  || p.x + p.width  <= f.position.x ||
      p.y >= f.position.y + f.height || p.y + p.height <= f.position.y ||
      p.z >= f.position.z + f.length || p.z + p.length <= f.position.z
  ) return;

  // Collision vector.
  vec3 collision = vec3(pos.x + p.width/2, pos.y + p.height/2, pos.z + p.length/2);
  collision -= f.position + vec3(f.width, f.height, f.length);

  vector<GLfloat> components {
    (collision.y > 0) ? collision.y : numeric_limits<GLfloat>::max(),
    (collision.y < 0) ? -collision.y : numeric_limits<GLfloat>::max(),
    (collision.x > 0) ? collision.x : numeric_limits<GLfloat>::max(),
    (collision.x < 0) ? -collision.x : numeric_limits<GLfloat>::max(),
    (collision.z > 0) ? collision.z : numeric_limits<GLfloat>::max(),
    (collision.z < 0) ? -collision.z : numeric_limits<GLfloat>::max(),
  };

  int min_index = -1;
  GLfloat minimum = numeric_limits<GLfloat>::max();
  for (int i = 0; i < components.size(); i++) {
    if (components[i] < minimum) {
      minimum = components[i];
      min_index = i;
    }
  }

  switch (min_index) {
    case 0: // Top.
      pos.y = f.position.y + f.height;
      break;
    case 1: // Bottom.
      pos.y = f.position.y - p.height;
      break;
    case 2: // Right.
      pos.x = f.position.x + f.width;
      break;
    case 3: // Left.
      pos.x = f.position.x - p.width;
      break;
    case 4: // Front.
      pos.z = f.position.z + f.length;
      break;
    case 5: // Back.
      pos.z = f.position.z - p.length;
      break;
  }
}

vec3 EntityManager::GetCollisionPointWithFloor(vec3& pos, vec3& direction, Floor& f, int& collision_direction) {
  // Top:    Y = f.y
  // Bottom: Y = f.y + f.height
  // Left:   X = f.x
  // Right:  X = f.x + f.width
  // Front:  Z = f.z
  // Back:   Z = f.z + f.length
 
  // vec3 A = 0, 0, 0.
  // vec3 B = 1, 1, 1.
  // A.x + B.x * k = 10
  // k = (10 - A.x) / B.x
  // Collision = A + B * k

  vector<GLfloat> planes {
    f.position.y,             // Top.
    f.position.y + f.height,  // Bottom.
    f.position.x,             // Left.
    f.position.x + f.width,   // Right.
    f.position.z,             // Front.
    f.position.z + f.length   // Back.
  };

  vector<GLfloat> collisions {
    (planes[0] - pos.y) / direction.y,
    (planes[1] - pos.y) / direction.y,
    (planes[2] - pos.x) / direction.x,
    (planes[3] - pos.x) / direction.x,
    (planes[4] - pos.z) / direction.z,
    (planes[5] - pos.z) / direction.z
  };

  vector<vec3> collision_points {
    pos + direction * collisions[0],
    pos + direction * collisions[1],
    pos + direction * collisions[2],
    pos + direction * collisions[3],
    pos + direction * collisions[4],
    pos + direction * collisions[5]
  };

  if (collision_points[0].x < f.position.x || collision_points[0].x > f.position.x + f.width  || collision_points[0].z < f.position.z || collision_points[0].z > f.position.z + f.length)
    collisions[0] = -1;

  if (collision_points[1].x < f.position.x || collision_points[1].x > f.position.x + f.width  || collision_points[1].z < f.position.z || collision_points[1].z > f.position.z + f.length)
    collisions[1] = -1;

  if (collision_points[2].y < f.position.y || collision_points[2].y > f.position.y + f.height || collision_points[2].z < f.position.z || collision_points[2].z > f.position.z + f.length)
    collisions[2] = -1;

  if (collision_points[3].y < f.position.y || collision_points[3].y > f.position.y + f.height || collision_points[3].z < f.position.z || collision_points[3].z > f.position.z + f.length)
    collisions[3] = -1;

  if (collision_points[4].x < f.position.x || collision_points[4].x > f.position.x + f.width  || collision_points[4].y < f.position.y || collision_points[4].y > f.position.y + f.height)
    collisions[4] = -1;

  if (collision_points[5].x < f.position.x || collision_points[5].x > f.position.x + f.width  || collision_points[5].y < f.position.y || collision_points[5].y > f.position.y + f.height)
    collisions[5] = -1;

  int min_index = -1;
  GLfloat minimum = numeric_limits<GLfloat>::max();
  for (int i = 0; i < 6; i++) {
    if (collisions[i] < 0) continue;
    if (collisions[i] < minimum) {
      minimum = collisions[i];
      min_index = i;
    } 
  }

  if (min_index == -1) 
    return vec3(0, 0, 0);

  collision_direction = min_index;
  return collision_points[min_index] - pos;
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
  vec3 direction = game_state_->camera().direction;

  vec3 point(0);
  GLfloat min_distance = 10.0f;
  Floor* colliding_floor = nullptr;
  int collision_direction = -1;
  for (auto& f : building_->floors()) {
    vec3 player_pos = game_state_->player().position;
   
    int cur_collision_direction = -1; 
    vec3 cur_point = GetCollisionPointWithFloor(player_pos, direction, f, cur_collision_direction);
    if (cur_point.x < 0.01f && cur_point.y < 0.01f && cur_point.z < 0.01f) continue;

    GLfloat distance = glm::length(cur_point);
    if (distance < min_distance) {
      min_distance = distance;
      point = cur_point;
      colliding_floor = &f;
      collision_direction = cur_collision_direction;
    }
  }

  direction /= glm::length(direction);

  if (colliding_floor) {
    switch (collision_direction) {
      case 0:
      case 1:
        min_distance -= 1.0 / direction.y;
        break;
      case 2:
      case 3:
        min_distance += 1.0 / direction.x;
        break;
      case 4:
      case 5: 
        min_distance -= (0.1 / direction.z);
        break;
    }
  }

  vec3 next_pos = game_state_->player().position + direction * min_distance;

  // next_pos, p.rotation_, p.highlighted, fbo.texture
  // Get floor and plane that collided. Adjust bounds accordingly.
  GLfloat rotation = glm::radians(int(4 * game_state_->player().h_angle / (PI * 2)) * 90.0f);

  auto& p = plots_[3];
  FBO fbo = renderer_->GetFBO(p.filename);
  renderer_->DrawHighlightedObject(
    "2d_plot", ProjectionMatrix, ViewMatrix, camera, 
    next_pos, rotation, p.highlighted, fbo.texture
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
