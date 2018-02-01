#include "sky_dome.hpp"

namespace Sibyl {

SkyDome::SkyDome(
  std::shared_ptr<Player> player,
  Shader shader
) : shader_(shader), player_(player) {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);

  CreateMesh();

  for (int i = 0; i < 2048; i++) {
    for (int j = 0; j < 2048; j++) {
      data_[i * 2048 * 3 + j * 3 + 0] = rand() % 255;
      data_[i * 2048 * 3 + j * 3 + 1] = rand() % 255;
      data_[i * 2048 * 3 + j * 3 + 2] = rand() % 255;
      
    }
  } 

  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, data_);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void SkyDome::CreateMesh() {
  float angle_step = (PI * 2) / NUM_POINTS_IN_CIRCLE;
  float y_step = DOME_RADIUS / NUM_CIRCLES;
  float y = DOME_RADIUS - y_step;

  vertices_.push_back(glm::vec3(0, DOME_RADIUS, 0));
  uvs_.push_back(glm::vec2(0.5, 0.5));
  for (int i = 0; i < NUM_CIRCLES; i++) {
    float radius = cos(asin(y / DOME_RADIUS)) * DOME_RADIUS;
    float uv_radius = (i + 1) * (1.0f / NUM_CIRCLES);
    float angle = 0;
    for (int j = 0; j < NUM_POINTS_IN_CIRCLE; j++) {
      float x = radius * cos(angle);
      float z = radius * sin(angle);
      vertices_.push_back(glm::vec3(x, y, z));
      // uvs_.push_back(glm::vec2((DOME_RADIUS + x) / (2 * DOME_RADIUS), (DOME_RADIUS + z) / (2 * DOME_RADIUS)));
      uvs_.push_back(glm::vec2(0.5 + uv_radius * cos(angle), 0.5 + uv_radius * sin(angle)));
      angle += angle_step;
    }
    y -= y_step;
  } 

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, uvs_.size() * sizeof(glm::vec2), &uvs_[0], GL_STATIC_DRAW);


  for (int j = 0; j < NUM_POINTS_IN_CIRCLE; j++) {
    indices_.push_back(0);
    int next_j = (j == NUM_POINTS_IN_CIRCLE - 1) ? 1 : j + 2;
    indices_.push_back(j + 1);
    indices_.push_back(next_j);
  }

  for (int i = 0; i < NUM_CIRCLES - 1; i++) {
    for (int j = 0; j < NUM_POINTS_IN_CIRCLE; j++) {
      int next_j = (j == NUM_POINTS_IN_CIRCLE - 1) ? 0 : j + 1;
      indices_.push_back(1 + i       * NUM_POINTS_IN_CIRCLE + j);
      indices_.push_back(1 + (i + 1) * NUM_POINTS_IN_CIRCLE + j);
      indices_.push_back(1 + i       * NUM_POINTS_IN_CIRCLE + next_j);
      indices_.push_back(1 + i       * NUM_POINTS_IN_CIRCLE + next_j);
      indices_.push_back(1 + (i + 1) * NUM_POINTS_IN_CIRCLE + j);
      indices_.push_back(1 + (i + 1) * NUM_POINTS_IN_CIRCLE + next_j);
    }
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices_.size() * sizeof(unsigned int), 
    &indices_[0], 
    GL_STATIC_DRAW
  );
}

void SkyDome::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glUniform1i(shader_.GetUniformId("SkyTextureSampler"), 0);

  glm::vec3 position = player_->position();
  position.y = -10000.0f;
  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position);
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
}

} // End of namespace.
