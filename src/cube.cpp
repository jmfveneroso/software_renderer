#include "cube.hpp"

using namespace std;

namespace Sibyl {

Cube::Cube(
  Shader shader
) : shader_(shader), position_(2050.0f, 215.0f, 2050.0f) {
  CreateCube();
}

void Cube::CreateCube() {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);
 
  float s = 1.0f;
  vector<glm::vec2> uvs;

  // Top face.
  vertices_.push_back(glm::vec3(-s, s, -s)); uvs.push_back(glm::vec2(0, 0)); indices_.push_back(0);
  vertices_.push_back(glm::vec3(-s, s, s )); uvs.push_back(glm::vec2(0, 1)); indices_.push_back(1);
  vertices_.push_back(glm::vec3(s , s, -s)); uvs.push_back(glm::vec2(1, 0)); indices_.push_back(2);
  vertices_.push_back(glm::vec3(s , s, -s)); uvs.push_back(glm::vec2(1, 0)); indices_.push_back(3);
  vertices_.push_back(glm::vec3(-s, s, s )); uvs.push_back(glm::vec2(0, 1)); indices_.push_back(4);
  vertices_.push_back(glm::vec3(s , s, s )); uvs.push_back(glm::vec2(1, 1)); indices_.push_back(5);

  // Front face.
  vertices_.push_back(glm::vec3(-s, s , s)); uvs.push_back(glm::vec2(0, 0)); indices_.push_back(6);
  vertices_.push_back(glm::vec3(-s, -s, s)); uvs.push_back(glm::vec2(0, 1)); indices_.push_back(7);
  vertices_.push_back(glm::vec3(s , s , s)); uvs.push_back(glm::vec2(1, 0)); indices_.push_back(8);
  vertices_.push_back(glm::vec3(s , s , s)); uvs.push_back(glm::vec2(1, 0)); indices_.push_back(9);
  vertices_.push_back(glm::vec3(-s, -s, s)); uvs.push_back(glm::vec2(0, 1)); indices_.push_back(10);
  vertices_.push_back(glm::vec3(s, -s , s)); uvs.push_back(glm::vec2(1, 1)); indices_.push_back(11);

  // Back face.
  vertices_.push_back(glm::vec3(s , s , -s)); uvs.push_back(glm::vec2(0, 0)); indices_.push_back(12);
  vertices_.push_back(glm::vec3(s , -s, -s)); uvs.push_back(glm::vec2(0, 1)); indices_.push_back(13);
  vertices_.push_back(glm::vec3(-s, s , -s)); uvs.push_back(glm::vec2(1, 0)); indices_.push_back(14);
  vertices_.push_back(glm::vec3(-s, s , -s)); uvs.push_back(glm::vec2(1, 0)); indices_.push_back(15);
  vertices_.push_back(glm::vec3(s , -s, -s)); uvs.push_back(glm::vec2(0, 1)); indices_.push_back(16);
  vertices_.push_back(glm::vec3(-s, -s, -s)); uvs.push_back(glm::vec2(1, 1)); indices_.push_back(17);

  // Right face.
  vertices_.push_back(glm::vec3(s, -s, s )); uvs.push_back(glm::vec2(0, 0)); indices_.push_back(18);
  vertices_.push_back(glm::vec3(s, -s, -s)); uvs.push_back(glm::vec2(0, 1)); indices_.push_back(19);
  vertices_.push_back(glm::vec3(s, s , s )); uvs.push_back(glm::vec2(1, 0)); indices_.push_back(20);
  vertices_.push_back(glm::vec3(s, s , s )); uvs.push_back(glm::vec2(1, 0)); indices_.push_back(21);
  vertices_.push_back(glm::vec3(s, -s, -s)); uvs.push_back(glm::vec2(0, 1)); indices_.push_back(22);
  vertices_.push_back(glm::vec3(s, s, -s )); uvs.push_back(glm::vec2(1, 1)); indices_.push_back(23);

  // Left face.
  vertices_.push_back(glm::vec3(-s, s, -s )); uvs.push_back(glm::vec2(0, 0)); indices_.push_back(24);
  vertices_.push_back(glm::vec3(-s, -s, -s)); uvs.push_back(glm::vec2(0, 1)); indices_.push_back(25);
  vertices_.push_back(glm::vec3(-s, s , s )); uvs.push_back(glm::vec2(1, 0)); indices_.push_back(26);
  vertices_.push_back(glm::vec3(-s, s , s )); uvs.push_back(glm::vec2(1, 0)); indices_.push_back(27);
  vertices_.push_back(glm::vec3(-s, -s, -s)); uvs.push_back(glm::vec2(0, 1)); indices_.push_back(28);
  vertices_.push_back(glm::vec3(-s, -s, s )); uvs.push_back(glm::vec2(1, 1)); indices_.push_back(29);

  // Bottom face.
  vertices_.push_back(glm::vec3(s , -s , -s)); uvs.push_back(glm::vec2(0, 0)); indices_.push_back(30);
  vertices_.push_back(glm::vec3( s, -s , s )); uvs.push_back(glm::vec2(0, 1)); indices_.push_back(31);
  vertices_.push_back(glm::vec3(-s , -s, -s)); uvs.push_back(glm::vec2(1, 0)); indices_.push_back(32);
  vertices_.push_back(glm::vec3(-s , -s, -s)); uvs.push_back(glm::vec2(1, 0)); indices_.push_back(33);
  vertices_.push_back(glm::vec3(s , -s , s )); uvs.push_back(glm::vec2(0, 1)); indices_.push_back(34);
  vertices_.push_back(glm::vec3(-s , -s, s )); uvs.push_back(glm::vec2(1, 1)); indices_.push_back(35);

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

void Cube::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  v_angle_ += 3.14f / 512;
  h_angle_ += 3.14f / 256;

  position_ += speed_;
  if (position_.y < 207) speed_ = glm::vec3(0, 0.1, 0);
  if (position_.y > 215) speed_ = glm::vec3(0, -0.1, 0);

  glUseProgram(shader_.program_id());

  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
  ModelMatrix *= glm::rotate(glm::mat4(1.0f), v_angle_, glm::vec3(1.0, 0.0, 0.0));
  ModelMatrix *= glm::rotate(glm::mat4(1.0f), h_angle_, glm::vec3(0.0, 1.0, 0.0));
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

} // End of namespace.
