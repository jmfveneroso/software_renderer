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
  LoadModel(filename);
}

void Solid::LoadModel(const std::string& filename) {
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals; 
  std::vector<glm::vec3> tangents;
  std::vector<glm::vec3> bitangents;
  ::loadOBJ(filename.c_str(), vertices_, uvs, normals);
  ::computeTangentBasis(
    vertices_, uvs, normals, // Input.
    tangents, bitangents    // Output.
  );

  std::vector<glm::vec3> indexed_vertices;
  std::vector<glm::vec2> indexed_uvs;
  std::vector<glm::vec3> indexed_normals;
  std::vector<glm::vec3> indexed_tangents;
  std::vector<glm::vec3> indexed_bitangents;
  ::indexVBO_TBN(
    vertices_, uvs, normals, tangents, bitangents, 
    indices_, indexed_vertices, indexed_uvs, 
    indexed_normals, indexed_tangents, indexed_bitangents
  );
  
  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &uv_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &normal_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

  glGenBuffers(1, &tangent_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, tangent_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &bitangent_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, bitangent_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned short), &indices_[0] , GL_STATIC_DRAW);
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

  glm::vec4 plane = vec4(0, -1, 0, 10000);
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
  
  // Index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, (void*) 0);

  shader_.Clear();
}

void Solid::Clean() {
    glDeleteBuffers(1, &vertex_buffer_);
    glDeleteBuffers(1, &uv_buffer_);
    glDeleteBuffers(1, &normal_buffer_);
    glDeleteBuffers(1, &element_buffer_);
    glDeleteBuffers(1, &tangent_buffer_);
    glDeleteBuffers(1, &bitangent_buffer_);
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
  LoadModel(filename);
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
  shader_.BindBuffer(vertex_buffer_, 0, 3);
  shader_.BindBuffer(uv_buffer_, 1, 2);
  shader_.BindBuffer(normal_buffer_, 2, 3);
  shader_.BindBuffer(tangent_buffer_, 3, 3);
  shader_.BindBuffer(bitangent_buffer_, 4, 3);
  
  // Draw.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, (void*)0);

  shader_.Clear();
}

} // End of namespace.
