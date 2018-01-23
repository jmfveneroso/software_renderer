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

TextureScreen::TextureScreen(
  Shader shader,
  GLuint texture_id
) : shader_(shader),
    texture_id_(texture_id), 
    position_(glm::vec3(0.0, 0.0, 0.0)) {
}

void TextureScreen::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
}

void TextureScreen::Clean() {
}

} // End of namespace.
