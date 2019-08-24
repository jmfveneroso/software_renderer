#include "building.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

Floor::Floor(
  Shader shader,
  glm::vec3 position,
  float width,
  float length
) : shader_(shader), 
    position_(position), 
    width_(width), 
    length_(length) {
  Init();
}

void Floor::Init() {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);

  float w = width_;
  float l = length_;
  float h = 0.5;
 
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
      p.x >= position_.x + width_  || p.x + p.width <= position_.x ||
      p.z >= position_.z + length_ || p.z + p.length <= position_.z
  ) return;

  if (p.y >= position_.y + 0.5 || p.y + p.height <= position_.y) 
    return;

  // Coming from top.
  if (prev_pos.y > player_pos.y) {
    player_pos.y = position_.y + 0.5 + 1.5;

  // Coming from bottom.
  } else {
    player_pos.y = position_.y - 1.5;
  }
}

Wall::Wall(
  Shader shader,
  glm::vec3 position,
  float rotation,
  float length, 
  float height
) : shader_(shader), 
    position_(position), 
    rotation_(rotation), 
    length_(length),
    height_(height) {
  Init();
}

void Wall::Init() {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);

  float l = length_;
  float h = height_;
  float t = 0.25;
 
  vector<vec3> v {
    // Back face.
    vec3(0, h, 0), vec3(l, h, 0), vec3(0, 0, 0), vec3(l, 0, 0),
    // Front face.
    vec3(0, h, t), vec3(l, h, t), vec3(0, 0, t), vec3(l, 0, t)
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
    vec2(0, 0), vec2(0, t), vec2(l, 0), vec2(l, t), // Top.
    vec2(0, 0), vec2(0, h), vec2(l, 0), vec2(l, h), // Back.
    vec2(0, 0), vec2(0, h), vec2(t, 0), vec2(t, h)  // Left.
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

void Wall::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());

  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
  ModelMatrix *= glm::rotate(glm::mat4(1.0f), (3.14f / 2) * -rotation_, glm::vec3(0.0, 1.0, 0.0));

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

void Wall::Collide(glm::vec3& player_pos, glm::vec3 prev_pos) {
  BoundingBox p = BoundingBox(player_pos.x - 0.35, player_pos.y - 1.5, player_pos.z - 0.35, 0.7, 1.5, 0.7);
  if (p.y >= position_.y + height_ || p.y + p.height <= position_.y) return;

  bool horizontal = rotation_ == 0 || rotation_ == 2;
  if (horizontal) {
    float x = position_.x;
    float z = position_.z;
    float w = length_;
    float l = 0.25;

    if (rotation_ == 2) {
      x -= w; z -= l;
    }

    if (
      p.x < x + w && p.x + p.width > x   &&
      p.z < z + l && p.z + p.length > z
    ) {
      if (prev_pos.z < player_pos.z)
        player_pos.z = z - (p.length/2);
      else
        player_pos.z = z + l;
    }
  } else {
    float x = position_.x;
    float z = position_.z;
    float w = 0.25;
    float l = length_;

    if (rotation_ == 3) {
      x -= w; z -= l;
    }

    if (
      p.x < x + w && p.x + p.width > x  &&
      p.z < z + l && p.z + p.length > z
    ) {
      if (prev_pos.x < player_pos.x)
        player_pos.x = x - (p.width/2);
      else
        player_pos.x = x + w;
    }
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
  // CreateFloor(vec3(1995, 205, 1995));
}

void Building::CreateFloor(glm::vec3 position) {
  walls_.push_back(Wall(shader_,   position + vec3(0, 0, 0), 0, 5, 6));
  walls_.push_back(Wall(shader_,   position + vec3(6, 0, 0), 0, 5, 6));
  walls_.push_back(Wall(shader_,   position + vec3(5, 2, 0), 0, 1, 4));

  walls_.push_back(Wall(shader_,   position + vec3(11, 0, 0.25), 1, 4.75, 6));
  walls_.push_back(Wall(shader_,   position + vec3(11, 0, 6), 1, 4.75, 6));
  walls_.push_back(Wall(shader_,   position + vec3(11, 4, 5), 1, 1, 2));
  walls_.push_back(Wall(shader_,   position + vec3(11, 0, 5), 1, 1, 1));

  walls_.push_back(Wall(shader_,   position + vec3(11, 0, 11), 2, 11, 6));

  walls_.push_back(Wall(shader_,   position + vec3(0, 0, 10.75), 3, 4.75, 6));
  walls_.push_back(Wall(shader_,   position + vec3(0, 0, 5), 3, 4.75, 6));
  walls_.push_back(Wall(shader_,   position + vec3(0, 4, 6), 3, 1, 2));
  walls_.push_back(Wall(shader_,   position + vec3(0, 0, 6), 3, 1, 1));

  floors_.push_back(Floor(shader_, position + vec3(0, 6, 0), 11, 9));
  floors_.push_back(Floor(shader_, position + vec3(0, 6, 9), 3, 2));
  floors_.push_back(Floor(shader_, position + vec3(9, 6, 9), 2, 2));
 
  // Stairs.
  float x = 0;
  for (int i = 0; i < 12; i++) {
    x += 0.5;
    floors_.push_back(Floor(shader_, position + vec3(9 - x, 6 - x, 9), 0.5, 2));
  }
}

void Building::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  for (auto& w : walls_)
    w.Draw(ProjectionMatrix, ViewMatrix, camera);

  for (auto& f : floors_)
    f.Draw(ProjectionMatrix, ViewMatrix, camera);
}

void Building::Collide(glm::vec3& player_pos, glm::vec3 prev_pos) {
  for (auto& w : walls_)
    w.Collide(player_pos, prev_pos);
  
  for (auto& f : floors_)
    f.Collide(player_pos, prev_pos);
}

} // End of namespace.
