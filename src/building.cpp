#include "building.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

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

Building::Building(
  Shader shader,
  float sx, 
  float sz,
  glm::vec3 position
) : shader_(shader), position_(position), sx_(sx), sz_(sz) {
  walls_.push_back(Wall(shader_, vec3(1995, 205, 1995), 0, 10, 6));
  walls_.push_back(Wall(shader_, vec3(2005, 205, 1995.25), 1, 9.5, 6));
  walls_.push_back(Wall(shader_, vec3(2005, 205, 2005), 2, 10, 6));
  walls_.push_back(Wall(shader_, vec3(1995, 205, 2004.75), 3, 9.5, 6));
}


void Building::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  for (auto& w : walls_)
    w.Draw(ProjectionMatrix, ViewMatrix, camera);
}

} // End of namespace.
