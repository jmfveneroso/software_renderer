#include "building.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

Object::Object(
  Shader shader,
  glm::vec3 position,
  const string& filename
) : shader_(shader), 
    position_(position) {
  Load(filename);
}

void Object::Load(const string& filename) {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);

  ifstream f(filename);
  if (!f.is_open()) return;

  vector<glm::vec3> vertex_lookup;
  vector<glm::vec2> uv_lookup;
  vector<glm::vec2> uvs;

  string line;
  while (getline(f, line)) {
    vector<string> tokens;
    boost::split(tokens, line, boost::is_any_of(" "));
    if (!tokens.size()) continue;

    string type = tokens[0];
    if (type == "v") {
      glm::vec3 vertex;
      vertex.x = boost::lexical_cast<float>(tokens[1]); 
      vertex.y = boost::lexical_cast<float>(tokens[2]);
      vertex.z = boost::lexical_cast<float>(tokens[3]);
      vertex_lookup.push_back(vertex);
    } else if (type == "vt") {
      glm::vec2 uv_coordinate;
      uv_coordinate.x = boost::lexical_cast<float>(tokens[1]); 
      uv_coordinate.y = boost::lexical_cast<float>(tokens[2]);
      uv_lookup.push_back(uv_coordinate);
    } else if (type == "f") {
      vector<unsigned int> vertex_ids;
      vector<unsigned int> uv_ids;
      for (int i = 1; i < tokens.size(); i++) {
        string& s = tokens[i];
        size_t j = s.find_first_of("/", 0);
        vertex_ids.push_back(boost::lexical_cast<unsigned int>(s.substr(0, j)) - 1); 

        size_t k = s.find_first_of("/", j+1);
        uv_ids.push_back(boost::lexical_cast<unsigned int>(s.substr(j+1, k-j-1)) - 1); 
      }
  
      for (int i = 0; i < 3; i++) { 
        vertices_.push_back(vertex_lookup[vertex_ids[i]]);
        uvs.push_back(uv_lookup[uv_ids[i]]);
        indices_.push_back(vertices_.size()-1);
      }
    }
  }
  f.close();

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_); glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices_.size() * sizeof(unsigned int), 
    &indices_[0], 
    GL_STATIC_DRAW
  );
}

void Object::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());

  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat4 MVP = ProjectionMatrix * ModelViewMatrix;

  glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);

  shader_.BindBuffer(vertex_buffer_, 0, 3);
  shader_.BindBuffer(uv_buffer_, 1, 2);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*) 0);

  shader_.Clear();
}

void Object::Collide(glm::vec3& player_pos, glm::vec3 prev_pos, bool& can_jump, glm::vec3& speed) {
  for (int i = 0; i < indices_.size(); i += 3) {
    vec3 v[3];
    v[0] = vertices_[indices_[i]];
    v[1] = vertices_[indices_[i+1]];
    v[2] = vertices_[indices_[i+2]];
  }
}

Floor::Floor(
  Shader shader,
  glm::vec3 position,
  float width,
  float height,
  float length
) : shader_(shader), 
    position_(position), 
    width_(width), 
    height_(height),
    length_(length) {
  Init();
}

void Floor::Init() {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);

  float w = width_;
  float l = length_;
  float h = height_;
 
  vector<vec3> v {
    // Back face.
    vec3(0, h, 0), vec3(w, h, 0), vec3(0, 0, 0), vec3(w, 0, 0),
    // Front face.
    vec3(0, h, l), vec3(w, h, l), vec3(0, 0, l), vec3(w, 0, l),
  };

  vertices_ = {
    v[0], v[4], v[1], v[1], v[4], v[5], // Top.
    v[1], v[3], v[0], v[0], v[3], v[2], // Back.
    v[0], v[2], v[4], v[4], v[2], v[6], // Left.
    v[5], v[7], v[1], v[1], v[7], v[3], // Right.
    v[4], v[6], v[5], v[5], v[6], v[7], // Front.
    v[6], v[2], v[7], v[7], v[2], v[3]  // Bottom.
  };

  vector<vec2> u = {
    vec2(0, 0), vec2(0, l), vec2(w, 0), vec2(w, l), // Top.
    vec2(0, 0), vec2(0, h), vec2(w, 0), vec2(w, h), // Back.
    vec2(0, 0), vec2(0, h), vec2(l, 0), vec2(l, h)  // Left.
  };

  vector<glm::vec2> uvs {
    u[0], u[1], u[2],  u[2],  u[1], u[3],  // Top.
    u[4], u[5], u[6],  u[6],  u[5], u[7],  // Back.
    u[8], u[9], u[10], u[10], u[9], u[11], // Left.
    u[8], u[9], u[10], u[10], u[9], u[11], // Right.
    u[4], u[5], u[6],  u[6],  u[5], u[7],  // Front.
    u[0], u[1], u[2],  u[2],  u[1], u[3]   // Bottom.
  };

  for (int i = 0; i < 36; i++) indices_.push_back(i);

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices_.size() * sizeof(unsigned int), 
    &indices_[0], 
    GL_STATIC_DRAW
  );
}

void Floor::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());

  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);

  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat4 MVP = ProjectionMatrix * ModelViewMatrix;

  glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);

  shader_.BindBuffer(vertex_buffer_, 0, 3);
  shader_.BindBuffer(uv_buffer_, 1, 2);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*) 0);
  shader_.Clear();
}

void Floor::Collide(glm::vec3& player_pos, glm::vec3 prev_pos, bool& can_jump, glm::vec3& speed) {
  // Player bounding box.
  BoundingBox p = BoundingBox(
    player_pos.x - 0.35, 
    player_pos.y - 1.5, 
    player_pos.z - 0.35, 
    0.7, 1.5, 0.7
  );

  // AABB collision.
  if (
      p.x >= position_.x + width_  || p.x + p.width  <= position_.x ||
      p.y >= position_.y + height_ || p.y + p.height <= position_.y ||
      p.z >= position_.z + length_ || p.z + p.length <= position_.z
  ) return;

  // Collision vector.
  vec3 collision = player_pos - prev_pos;

  // Collision point between the player and each box face in collision 
  // vector coordinates.
  vector<double> collision_magnitude {
    (position_.y + height_ + 1.50 - prev_pos.y) / collision.y, // Top.
    (position_.y - prev_pos.y                 ) / collision.y, // Bottom.
    (position_.x + width_  + 0.35 - prev_pos.x) / collision.x, // Right.
    (position_.x - 0.35 - prev_pos.x          ) / collision.x, // Left.
    (position_.z + length_ + 0.35 - prev_pos.z) / collision.z, // Front.
    (position_.z - 0.35 - prev_pos.z          ) / collision.z  // Back.
  };

  // Select the face for which the collision displacement is minimum
  // when moving the player only along the collision vector.
  int min_index = -1;
  double min_value = numeric_limits<double>::max();
  for (int i = 0; i < 6; i++) {
    double k = collision_magnitude[i];
    if (k < 0.0 || k > 1.0) continue;

    if (min_index == -1 || k < min_value) {
      min_index = i;
      min_value = k;
    }
  }

  // Displace the player such that it is no longer in collision with
  // the selected face.
  switch (min_index) {
    case 0:
      can_jump = true;
      speed.y = 0;
    case 1:
      player_pos.y = prev_pos.y + min_value * collision.y;
      player_pos.y += ((min_index % 2) ? -0.0001 : 0.0001);
      break;
    case 2:
    case 3:
      player_pos.x = prev_pos.x + min_value * collision.x;
      player_pos.x += ((min_index % 2) ? -0.0001 : 0.0001);
      break;
    case 4:
    case 5:
      player_pos.z = prev_pos.z + min_value * collision.z;
      player_pos.z += ((min_index % 2) ? -0.0001 : 0.0001);
      break;
    default:
      break;
  }
}

Building::Building(
  Shader shader,
  Shader shader2,
  float sx, 
  float sz,
  glm::vec3 position
) : shader_(shader), shader2_(shader2), position_(position), sx_(sx), sz_(sz) {
  float s = 14.0f;
  float t = 0.25f;
  CreateFloor(vec3(1995, 205,   1995), s, true );
  CreateFloor(vec3(1995, 211.5, 1995), s, false);
  CreateFloor(vec3(1995, 218,   1995), s, false);

  vec3 pos = vec3(1995, 205, 1995);
  floors_.push_back(Floor(shader_, pos + vec3(-t,  19.5, -t  ), s+1+2*t, 1, t  ));
  floors_.push_back(Floor(shader_, pos + vec3(s+1, 19.5, 0   ), t,       1, s+1));
  floors_.push_back(Floor(shader_, pos + vec3(-t,  19.5, s+1 ), s+1+2*t, 1, t  ));
  floors_.push_back(Floor(shader_, pos + vec3(-t,  19.5, 0   ), t,       1, s+1));

  floors_.push_back(Floor(shader_, pos + vec3(4.25,  0, 2), 6.75, 1, 0.5));
  floors_.push_back(Floor(shader_, pos + vec3(4,  0, 2), 0.25, 4, 13));

  platform_ = Object(shader_, vec3(1970, 205.5, 1990), "meshes/book_stand.obj");
  painting_ = WallPainting(shader2_, vec3(1996, 208, 1995));

  painting_.DrawToTexture();
  painting_.DrawToTexture();
}

void Building::CreateFloor(glm::vec3 position, float s, bool door) {
  float t = 0.25;

  if (door) {
    floors_.push_back(Floor(shader_, position + vec3(-t,    0, -t), s/2+t, 6, t));
    floors_.push_back(Floor(shader_, position + vec3(s/2+1, 0, -t), s/2+t, 6, t));
    floors_.push_back(Floor(shader_, position + vec3(s/2,   2, -t), 1,     4, t));
  } else {
    floors_.push_back(Floor(shader_, position + vec3(-t, 0, -t), s+1+2*t, 6, t));
  }

  floors_.push_back(Floor(shader_, position + vec3(s+1, 0, 0),     t, 6, s/2));
  floors_.push_back(Floor(shader_, position + vec3(s+1, 0, s/2+1), t, 6, s/2));
  floors_.push_back(Floor(shader_, position + vec3(s+1, 4, s/2),   t, 2, 1  ));
  floors_.push_back(Floor(shader_, position + vec3(s+1, 0, s/2),   t, 1, 1  ));

  floors_.push_back(Floor(shader_, position + vec3(-t, 0, s+1), s+1+2*t, 6, t));

  floors_.push_back(Floor(shader_, position + vec3(-t, 0, 0    ), t, 6, s/2));
  floors_.push_back(Floor(shader_, position + vec3(-t, 0, s/2+1), t, 6, s/2));
  floors_.push_back(Floor(shader_, position + vec3(-t, 4, s/2  ), t, 2, 1  ));
  floors_.push_back(Floor(shader_, position + vec3(-t, 0, s/2  ), t, 1, 1  ));

  floors_.push_back(Floor(shader_, position + vec3(-t,    6, -t   ), s+1+2*t, 0.5, s-1+t));
  floors_.push_back(Floor(shader_, position + vec3(-t,    6, s-1-t), s-1-6+t, 0.5, 2+2*t));
  floors_.push_back(Floor(shader_, position + vec3(s-1-6, 6, s-1+2), 6,       0.5, t    ));
  floors_.push_back(Floor(shader_, position + vec3(s-1,   6, s-1  ), 2+t,     0.5, 2+t  ));
 
  // Stairs.
  float x = 0;
  for (int i = 0; i < 12; i++) {
    x += 0.5;
    floors_.push_back(Floor(shader_, position + vec3(s-1 - x, 6 - x, s-1), 0.5, 0.5, 2));
  }
}

void Building::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  painting_.Draw(ProjectionMatrix, ViewMatrix, camera);

  for (auto& f : floors_)
    f.Draw(ProjectionMatrix, ViewMatrix, camera);

  platform_.Draw(ProjectionMatrix, ViewMatrix, camera);
}

void Building::Collide(glm::vec3& player_pos, glm::vec3 prev_pos, bool& can_jump, glm::vec3& speed) {
  platform_.Collide(player_pos, prev_pos, can_jump, speed);

  for (auto& f : floors_)
    f.Collide(player_pos, prev_pos, can_jump, speed);
}

} // End of namespace.
