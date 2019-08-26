#include "building.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

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

void Floor::Collide(glm::vec3& player_pos, glm::vec3 prev_pos) {
  BoundingBox p = BoundingBox(player_pos.x - 0.35, player_pos.y - 1.5, player_pos.z - 0.35, 0.7, 1.5, 0.7);

  if (
      p.x >= position_.x + width_  || p.x + p.width  <= position_.x ||
      p.y >= position_.y + height_ || p.y + p.height <= position_.y ||
      p.z >= position_.z + length_ || p.z + p.length <= position_.z
  ) return;

  vec3 collision_v = player_pos - prev_pos;
  vector<float> k_arr(6, numeric_limits<float>::max());

  // Top Face Y.
  float f = position_.y + height_ + 1.5;
  float k = (f - prev_pos.y) / collision_v.y;
  if (k >= 0.0 && k <= 1.0) k_arr[0] = k;

  // Bottom Face Y.
  f = position_.y;
  k = (f - prev_pos.y) / collision_v.y;
  if (k >= 0.0 && k <= 1.0) k_arr[1] = k;

  // Right Face X.
  f = position_.x + width_ + 0.35;
  k = (f - prev_pos.x) / collision_v.x;
  if (k >= 0.0 && k <= 1.0) k_arr[2] = k;

  // Left Face X.
  f = position_.x - 0.35;
  k = (f - prev_pos.x) / collision_v.x;
  if (k >= 0.0 && k <= 1.0) k_arr[3] = k;

  // Front Face Z.
  f = position_.z + length_ + 0.35;
  k = (f - prev_pos.z) / collision_v.z;
  if (k >= 0.0 && k <= 1.0) k_arr[4] = k;

  // Back Face Z.
  f = position_.z - 0.35;
  k = (f - prev_pos.z) / collision_v.z;
  if (k >= 0.0 && k <= 1.0) k_arr[5] = k;

  int min_index = 0;
  float min_value = k_arr[0];

  for (int i = 1; i < 6; i++) {
    if (k_arr[i] < min_value) {
      min_index = i;
      min_value = k_arr[i];
    }
  } 

  k = k_arr[min_index] - 0.0001;
  switch (min_index) {
    case 0: // Top.
      player_pos.y = prev_pos.y + k * collision_v.y;
      break;
    case 1: // Bottom.
      player_pos.y = prev_pos.y + k * collision_v.y;
      break;
    case 2: // Right.
      player_pos.x = prev_pos.x + k * collision_v.x;
      break;
    case 3: // Left.
      player_pos.x = prev_pos.x + k * collision_v.x;
      break;
    case 4: // Front.
      player_pos.z = prev_pos.z + k * collision_v.z;
      break;
    case 5: // Back.
      player_pos.z = position_.z - 0.35;
      player_pos.z = prev_pos.z + k * collision_v.z;
      break;
    default:
      break;
  }
}

Building::Building(
  Shader shader,
  float sx, 
  float sz,
  glm::vec3 position
) : shader_(shader), position_(position), sx_(sx), sz_(sz) {
  CreateFloor(vec3(1995, 205, 1995));
  CreateFloor(vec3(1995, 211.5, 1995));
  CreateFloor(vec3(1995, 218, 1995));

  vec3 pos = vec3(1995, 205, 1995);
  floors_.push_back(Floor(shader_, pos + vec3(0, 19.5, 0), 11, 1, 0.25));
  floors_.push_back(Floor(shader_, pos + vec3(10.75, 19.5, 0), 0.25, 1, 10.5));
  floors_.push_back(Floor(shader_, pos + vec3(0, 19.5, 10.75), 11, 1, 0.25));
  floors_.push_back(Floor(shader_, pos + vec3(0, 19.5, 0.25), 0.25, 1, 10.5));
}

void Building::CreateFloor(glm::vec3 position) {
  floors_.push_back(Floor(shader_, position + vec3(0, 0, 0), 5, 6, 0.25));
  floors_.push_back(Floor(shader_, position + vec3(6, 0, 0), 5, 6, 0.25));
  floors_.push_back(Floor(shader_, position + vec3(5, 2, 0), 1, 4, 0.25));

  floors_.push_back(Floor(shader_, position + vec3(11, 0, 0.25), 0.25, 6, 4.75));
  floors_.push_back(Floor(shader_, position + vec3(11, 0, 6),    0.25, 6, 4.75));
  floors_.push_back(Floor(shader_, position + vec3(11, 4, 5),    0.25, 2, 1));
  floors_.push_back(Floor(shader_, position + vec3(11, 0, 5),    0.25, 1, 1));

  floors_.push_back(Floor(shader_, position + vec3(0, 0, 11), 11, 6, 0.25));

  floors_.push_back(Floor(shader_, position + vec3(0, 0, 0.25), 0.25, 6, 4.75));
  floors_.push_back(Floor(shader_, position + vec3(0, 0, 6),    0.25, 6, 4.75));
  floors_.push_back(Floor(shader_, position + vec3(0, 4, 5),    0.25, 2, 1));
  floors_.push_back(Floor(shader_, position + vec3(0, 0, 5),    0.25, 1, 1));

  floors_.push_back(Floor(shader_, position + vec3(0, 6, 0), 11, 0.5, 9));
  floors_.push_back(Floor(shader_, position + vec3(0, 6, 9), 3,  0.5, 2));
  floors_.push_back(Floor(shader_, position + vec3(9, 6, 9), 2,  0.5, 2));
 
  // Stairs.
  float x = 0;
  for (int i = 0; i < 12; i++) {
    x += 0.5;
    floors_.push_back(Floor(shader_, position + vec3(9 - x, 6 - x, 9), 0.5, 0.5, 2));
  }
}

void Building::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  for (auto& f : floors_)
    f.Draw(ProjectionMatrix, ViewMatrix, camera);
}

void Building::Collide(glm::vec3& player_pos, glm::vec3 prev_pos) {
  for (auto& f : floors_)
    f.Collide(player_pos, prev_pos);
}

} // End of namespace.
