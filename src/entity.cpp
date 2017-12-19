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

Terrain::Terrain(
  Shader shader, 
  GLuint diffuse_texture_id, 
  GLuint normal_texture_id, 
  GLuint specular_texture_id
) : shader_(shader),
    diffuse_texture_id_(diffuse_texture_id), 
    normal_texture_id_(normal_texture_id), 
    specular_texture_id_(specular_texture_id),
    position_(glm::vec3(0.0, 0.0, 0.0)) {
  CreateTiles();
}

unsigned int Terrain::AddVertex(float x, float y, float u, float v) {
  float side = 10;
  x = 500 + x * side + u * side;
  y = y * side + v * side;

  float factor = 0.005;
  float factor2 = 50;
  float height = factor2 * (sin(x * factor) + sin(y * factor)); 
  indexed_vertices_.push_back(glm::vec3(x, height, y));
  indexed_uvs_.push_back(glm::vec2(u, v));
  indexed_normals_.push_back(glm::vec3(0, 1, 0));
  indexed_tangents_.push_back(glm::vec3(0, 0, 0));
  indexed_bitangents_.push_back(glm::vec3(0, 0, 0));
  return indexed_vertices_.size() - 1;
}

void Terrain::CreateTiles() {
  for (int x = 0; x < 1000; x++) {
    for (int y = 0; y < 1000; y++) {
      unsigned int v1 = AddVertex(x, y, 0, 0);
      unsigned int v2 = AddVertex(x, y, 0, 1);
      unsigned int v3 = AddVertex(x, y, 1, 1);
      unsigned int v4 = AddVertex(x, y, 1, 0);

      indices_.push_back(v1);
      indices_.push_back(v2);
      indices_.push_back(v3);
      indices_.push_back(v1);
      indices_.push_back(v3);
      indices_.push_back(v4);
    }
  }
  std::cout << "indices: " << indices_.size() << std::endl;

  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_vertices_.size() * sizeof(glm::vec3), &indexed_vertices_[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &uv_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_uvs_.size() * sizeof(glm::vec2), &indexed_uvs_[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &normal_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_normals_.size() * sizeof(glm::vec3), &indexed_normals_[0], GL_STATIC_DRAW);

  glGenBuffers(1, &tangent_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, tangent_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_tangents_.size() * sizeof(glm::vec3), &indexed_tangents_[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &bitangent_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, bitangent_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_bitangents_.size() * sizeof(glm::vec3), &indexed_bitangents_[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned short), &indices_[0] , GL_STATIC_DRAW);
}

void Terrain::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
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
  shader_.BindBuffer(vertex_buffer_, 0, 3);
  shader_.BindBuffer(uv_buffer_, 1, 2);
  shader_.BindBuffer(normal_buffer_, 2, 3);
  shader_.BindBuffer(tangent_buffer_, 3, 3);
  shader_.BindBuffer(bitangent_buffer_, 4, 3);
  
  // Index buffer.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);

  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*) 0);
  shader_.Clear();
}

void Terrain::Clean() {
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteBuffers(1, &uv_buffer_);
  glDeleteBuffers(1, &normal_buffer_);
  glDeleteBuffers(1, &element_buffer_);
  glDeleteBuffers(1, &tangent_buffer_);
  glDeleteBuffers(1, &bitangent_buffer_);
}

} // End of namespace.
