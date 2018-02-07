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
  Shader shader, GLuint depth_map,
  glm::vec3 p1, glm::vec3 p2
) : shader_(shader),
    depth_map_(depth_map),
    position_(glm::vec3(0.0, 8000.0, 0.0)) {
  float s = 500.0f;

  glm::vec3 v[8];
  v[0] = glm::vec3(-s,  s, -s);
  v[1] = glm::vec3(-s,  s,  s);
  v[2] = glm::vec3( s,  s, -s);
  v[3] = glm::vec3( s,  s,  s);
  v[4] = glm::vec3(-s, -s, -s);
  v[5] = glm::vec3(-s, -s,  s);
  v[6] = glm::vec3( s, -s, -s);
  v[7] = glm::vec3( s, -s,  s);

  planes_.push_back(TestPlane(shader, depth_map, std::vector<glm::vec3>({ v[0], v[1], v[3], v[2] })));
  planes_.push_back(TestPlane(shader, depth_map, std::vector<glm::vec3>({ v[1], v[5], v[7], v[3] })));
  planes_.push_back(TestPlane(shader, depth_map, std::vector<glm::vec3>({ v[2], v[6], v[4], v[0] })));
  planes_.push_back(TestPlane(shader, depth_map, std::vector<glm::vec3>({ v[7], v[6], v[2], v[3] })));
  planes_.push_back(TestPlane(shader, depth_map, std::vector<glm::vec3>({ v[0], v[4], v[5], v[1] })));
  planes_.push_back(TestPlane(shader, depth_map, std::vector<glm::vec3>({ v[6], v[7], v[5], v[4] })));
}

void Cube::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  v_angle_ += 3.14f / 512;
  h_angle_ += 3.14f / 256;
  position_ += speed_;
  if (position_.y < 8000) {
    speed_ = glm::vec3(0, 5, 0);
  } else if (position_.y > 12000) {
    speed_ = glm::vec3(0, -5, 0);
  }

  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
  ModelMatrix *= glm::rotate(glm::mat4(1.0f), v_angle_, glm::vec3(1.0, 0.0, 0.0));
  ModelMatrix *= glm::rotate(glm::mat4(1.0f), h_angle_, glm::vec3(0.0, 1.0, 0.0));
  for (auto& plane : planes_) {
    plane.set_model_matrix(ModelMatrix);
    plane.Draw(ProjectionMatrix, ViewMatrix, camera);
  }
}

TestPlane::TestPlane(
  Shader shader, GLuint depth_map,
  std::vector<glm::vec3> points
) : shader_(shader),
    depth_map_(depth_map),
    position_(glm::vec3(0.0, 10000.0, 0.0)) {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &element_buffer_);
 
  vertices_ = points;
  for (int i = 1; i < vertices_.size() - 1; i++) {
    indices_.push_back(0);
    indices_.push_back(i);
    indices_.push_back(i + 1);
  }

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices_.size() * sizeof(unsigned int), 
    &indices_[0], 
    GL_STATIC_DRAW
  );

  model_matrix_ = glm::translate(glm::mat4(1.0), position_);
}

void TestPlane::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glUseProgram(shader_.program_id());

  glm::mat4 ModelViewMatrix = ViewMatrix * model_matrix_;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * model_matrix_;

  glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &model_matrix_[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader_.GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
  shader_.BindTexture("DepthMap", depth_map_);

  shader_.BindBuffer(vertex_buffer_, 0, 3);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*) 0);
  shader_.Clear();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);
}

} // End of namespace.
