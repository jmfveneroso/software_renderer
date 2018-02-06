#include "entity.hpp"

namespace Sibyl {

Solid::Solid(
  const std::string& filename, 
  Shader shader, 
  GLuint diffuse_texture_id, 
  GLuint normal_texture_id, 
  GLuint specular_texture_id
) : shader_(shader),
    diffuse_texture_id_(diffuse_texture_id), 
    normal_texture_id_(normal_texture_id), 
    specular_texture_id_(specular_texture_id),
    position_(glm::vec3(0.0, 0.0, 0.0)) {
  mesh_.LoadObj(filename);
}

void Solid::BindBuffers() {
  // Buffers.
  shader_.BindBuffer(mesh_.vertex_buffer(), 0, 3);
  shader_.BindBuffer(mesh_.uv_buffer(), 1, 2);
  shader_.BindBuffer(mesh_.normal_buffer(), 2, 3);
  shader_.BindBuffer(mesh_.tangent_buffer(), 3, 3);
  shader_.BindBuffer(mesh_.bitangent_buffer(), 4, 3);
  
  // Index buffer.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_.element_buffer());
}

void Solid::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());

  // Uniforms.
  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader_.GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

  glm::vec3 lightPos = glm::vec3(0, 2000, 0);
  glUniform3f(shader_.GetUniformId("LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);

  glm::vec4 plane = glm::vec4(0, -1, 0, 10000);
  glUniform4fv(shader_.GetUniformId("plane"), 1, (float*) &plane);
  glUniform1i(shader_.GetUniformId("use_normals"), false);
  glUniform1i(shader_.GetUniformId("water_fog"), false);

  // Textures.
  shader_.BindTexture("DiffuseTextureSampler", diffuse_texture_id_);
  shader_.BindTexture("NormalTextureSampler", normal_texture_id_);
  shader_.BindTexture("SpecularTextureSampler", specular_texture_id_);

  // Buffers.
  BindBuffers();

  glDrawElements(GL_TRIANGLES, mesh_.indices().size(), GL_UNSIGNED_SHORT, (void*) 0);
  shader_.Clear();
}

void Solid::Clean() {
  mesh_.Clean();
}

float Water::move_factor = 0;

Water::Water(
  const std::string& filename, 
  Shader shader, 
  GLuint diffuse_texture_id, 
  GLuint normal_texture_id, 
  GLuint reflection_texture_id,
  GLuint refraction_texture_id,
  GLuint refraction_depth_texture_id
) : Solid(filename, shader, diffuse_texture_id, normal_texture_id, 0),
    reflection_texture_id_(reflection_texture_id),
    refraction_texture_id_(refraction_texture_id),
    refraction_depth_texture_id_(refraction_depth_texture_id) {
}

void Water::UpdateMoveFactor(float seconds) {
  move_factor += WAVE_SPEED * seconds;
  move_factor = fmod(move_factor, 1);
}

void Water::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());

  // Uniforms.
  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader_.GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

  glm::vec3 lightPos = glm::vec3(0, 2000, 0);
  glUniform3f(shader_.GetUniformId("LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);

  glUniform3fv(shader_.GetUniformId("cameraPosition"), 1, (float*) &camera);
  glUniform1f(shader_.GetUniformId("moveFactor"), Water::move_factor);

  // Textures.
  shader_.BindTexture("ReflectionTextureSampler", reflection_texture_id_);
  shader_.BindTexture("RefractionTextureSampler", refraction_texture_id_);
  shader_.BindTexture("dudvMap", diffuse_texture_id_);
  shader_.BindTexture("normalMap", normal_texture_id_);
  shader_.BindTexture("depthMap", refraction_depth_texture_id_);

  // Buffers.
  BindBuffers();

  glDrawElements(GL_TRIANGLES, mesh_.indices().size(), GL_UNSIGNED_SHORT, (void*) 0);
  shader_.Clear();
}

Cube::Cube(
  Shader shader
) : shader_(shader),
    position_(glm::vec3(0.0, 15000.0, 0.0)) {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);
 
  float s = 2000.0f;
  std::vector<glm::vec2> uvs;

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
  position_ += speed_;
  if (position_.y < 4000) {
    speed_ = glm::vec3(0, 5, 0);
  }
  if (position_.y > 8000) {
    speed_ = glm::vec3(0, -5, 0);
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glUseProgram(shader_.program_id());

  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader_.GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

  shader_.BindBuffer(vertex_buffer_, 0, 3);
  shader_.BindBuffer(uv_buffer_, 1, 2);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*) 0);
  shader_.Clear();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);
}

void Cube::Clean() {
}

} // End of namespace.
