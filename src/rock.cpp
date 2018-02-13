#include "rock.hpp"

namespace Sibyl {

Rock::Rock(Shader shader, GLuint texture) 
  : shader_(shader), texture_(texture) {
  num_circles_ = 64;
  num_points_in_circle_ = 128;
  dome_radius_ = 1000.0f;
  Init();
}

void Rock::Init() {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &normal_buffer_);
  glGenBuffers(5, element_buffers_);

  float angle_step = (PI * 2) / num_points_in_circle_;

  vertices_.push_back(glm::vec3(0, dome_radius_, 0));
  uvs_.push_back(glm::vec2(0.5, 0.5));

  float v_angle_step = (PI / 2) / num_circles_;
  float v_angle = PI / 2 - v_angle_step;
  for (int i = 0; i < 2 * num_circles_ - 1; i++) {
    float angle = 0;
    float y = sin(v_angle) * dome_radius_;

    if (v_angle < 0.001f && v_angle > -0.001f) v_angle = 0;
    float radius = cos(v_angle) * dome_radius_;
    float uv_radius = (i + 1) * (1.0f / num_circles_);
    if (i >= num_circles_) {
      uv_radius = (num_circles_ * 2 - (i + 1)) * (1.0f / num_circles_);
    }

    for (int j = 0; j < num_points_in_circle_; j++) {
      float x = radius * cos(angle);
      float z = radius * sin(angle);
      vertices_.push_back(glm::vec3(x, y, z));
      uvs_.push_back(glm::vec2(0.5 + uv_radius * cos(angle), 0.5 + uv_radius * sin(angle)));
      angle += angle_step;
    }

    v_angle -= v_angle_step;
  } 

  vertices_.push_back(glm::vec3(0, -dome_radius_, 0));
  uvs_.push_back(glm::vec2(0.5, 0.5));

  Flatten();
  CalculateNormals();

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, uvs_.size() * sizeof(glm::vec2), &uvs_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glBufferData(GL_ARRAY_BUFFER, normals_.size() * sizeof(glm::vec3), &normals_[0], GL_STATIC_DRAW);

  for (int lod = 4; lod >= 0; lod--) {
    CalculateIndices(lod);
  }

  float r = 0.2 * (float(rand() % 200) - 100) / 100;
  float g = 0.2 * (float(rand() % 200) - 100) / 100;
  float b = 0.2 * (float(rand() % 200) - 100) / 100;
  rock_color_ = glm::vec3(0.5 + r, 0.5 + g, 0.5 + b);
}

void Rock::CalculateIndices(int lod) {
  indices_.clear();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffers_[lod]);
  int original_lod = lod;
  lod = 1 << lod;

  for (int j = 0; j < num_points_in_circle_ / lod; j++) {
    indices_.push_back(0);
    int next_j = (j == num_points_in_circle_ / lod - 1) ? 1 : 1 + j * lod + lod;
    indices_.push_back(next_j);
    indices_.push_back(1 + j * lod);
  }

  for (int i = 0; i < num_circles_ / lod - 1; i++) {
    for (int j = 0; j < num_points_in_circle_ / lod; j++) {
      int next_j = (j == num_points_in_circle_ / lod - 1) ? 0 : j * lod + lod;
      indices_.push_back(1 + i       * num_points_in_circle_ * lod + j * lod);
      indices_.push_back(1 + i       * num_points_in_circle_ * lod + next_j);
      indices_.push_back(1 + (i + 1) * num_points_in_circle_ * lod + j * lod);
      indices_.push_back(1 + (i + 1) * num_points_in_circle_ * lod + j * lod);
      indices_.push_back(1 + i       * num_points_in_circle_ * lod + next_j);
      indices_.push_back(1 + (i + 1) * num_points_in_circle_ * lod + next_j);
    }
  }

  if (original_lod == 1) {
    int i  = num_circles_ - 2;
    int i2 = num_circles_ - 1;
    for (int j = 0; j < num_points_in_circle_ / lod; j++) {
      int next_j = (j == num_points_in_circle_ / lod - 1) ? 0 : j * lod + lod;
      indices_.push_back(1 + i  * num_points_in_circle_ + j * lod);
      indices_.push_back(1 + i  * num_points_in_circle_ + next_j);
      indices_.push_back(1 + i2 * num_points_in_circle_ + j * lod);
      indices_.push_back(1 + i2 * num_points_in_circle_ + j * lod);
      indices_.push_back(1 + i  * num_points_in_circle_ + next_j);
      indices_.push_back(1 + i2 * num_points_in_circle_ + next_j);
    }

    i  = num_circles_ - 1;
    i2 = num_circles_;
    for (int j = 0; j < num_points_in_circle_ / lod; j++) {
      int next_j = (j == num_points_in_circle_ / lod - 1) ? 0 : j * lod + lod;
      indices_.push_back(1 + i  * num_points_in_circle_ + j * lod);
      indices_.push_back(1 + i  * num_points_in_circle_ + next_j);
      indices_.push_back(1 + i2 * num_points_in_circle_ + j * lod);
      indices_.push_back(1 + i2 * num_points_in_circle_ + j * lod);
      indices_.push_back(1 + i  * num_points_in_circle_ + next_j);
      indices_.push_back(1 + i2 * num_points_in_circle_ + next_j);
    }
  } else {
    int i = num_circles_ / lod - 1;
    for (int j = 0; j < num_points_in_circle_ / lod; j++) {
      int next_j = (j == num_points_in_circle_ / lod - 1) ? 0 : j * lod + lod;
      indices_.push_back(1 + i       * num_points_in_circle_ * lod + j * lod);
      indices_.push_back(1 + i       * num_points_in_circle_ * lod + next_j);
      indices_.push_back(1 + (i + 1) * num_points_in_circle_ * lod + j * lod);
      indices_.push_back(1 + (i + 1) * num_points_in_circle_ * lod + j * lod);
      indices_.push_back(1 + i       * num_points_in_circle_ * lod + next_j);
      indices_.push_back(1 + (i + 1) * num_points_in_circle_ * lod + next_j);
    }
  }

  for (int i = num_circles_ / lod; i < 2 * (num_circles_ / lod) - 1; i++) {
    for (int j = 0; j < num_points_in_circle_ / lod; j++) {
      int next_j = (j == num_points_in_circle_ / lod - 1) ? 0 : j * lod + lod;
      indices_.push_back(1 + i       * num_points_in_circle_ * lod + j * lod);
      indices_.push_back(1 + i       * num_points_in_circle_ * lod + next_j);
      indices_.push_back(1 + (i + 1) * num_points_in_circle_ * lod + j * lod);
      indices_.push_back(1 + (i + 1) * num_points_in_circle_ * lod + j * lod);
      indices_.push_back(1 + i       * num_points_in_circle_ * lod + next_j);
      indices_.push_back(1 + (i + 1) * num_points_in_circle_ * lod + next_j);
    }
  }

  int i = 2 * (num_circles_ / lod) - 1;
  for (int j = 0; j < num_points_in_circle_ / lod; j++) {
    int next_j = (j == num_points_in_circle_ / lod - 1) ? 0 : j * lod + lod;
    indices_.push_back(vertices_.size() - 1);
    indices_.push_back(1 + (i * num_points_in_circle_ * lod) + j * lod);
    indices_.push_back(1 + (i * num_points_in_circle_ * lod) + next_j);
  }

  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices_.size() * sizeof(unsigned int), 
    &indices_[0], 
    GL_STATIC_DRAW
  );
  sizes_[original_lod] = indices_.size();
}

void Rock::Flatten() {
  int dome_radius = 1000.0f;

  int num_cuts = rand() % 10 + 1;
  for (int i = 0; i < num_cuts; i++) {
    float distance = dome_radius / 2 + (float(rand() % 100) / 100) * dome_radius / 2;
    float h_angle = 2 * PI * (float(rand() % 100) / 100);
    float v_angle = 2 * PI * (float(rand() % 100) / 100);

    glm::vec3 point(
      cos(v_angle) * sin(h_angle) * distance,
      sin(v_angle) * distance,
      cos(v_angle) * cos(h_angle) * distance
    );

    Plane plane(
      point,
      glm::vec3(0, 0, 0),
      glm::vec3(0, 0, 0),
      glm::normalize(glm::vec3(0, 0, 0) - point)
    );
    Geometry::FlattenPolygon(vertices_, plane);
  }

  for (int i = 0; i < vertices_.size(); i++) {
    float strength = 0.25;
    float amplitude = 0.0005f;

    float noise = strength * noise_.noise(1.0f + vertices_[i].x * amplitude, 1.0f + vertices_[i].y * amplitude);
    noise += strength * noise_.noise(1.0f + vertices_[i].z * amplitude, 1.0f + vertices_[i].x * amplitude);
    noise += 0.08 * noise_.noise(1.0f + vertices_[i].z * 0.002, 1.0f + vertices_[i].x * 0.002);

    vertices_[i] *= (1.0f - noise);
  }
}

void Rock::CalculateNormals() {
  normals_.push_back(glm::vec3(0, 1, 0));
  for (int i = 0; i < 2 * (num_circles_ - 1) - 1; i++) {
    for (int j = 0; j < num_points_in_circle_; j++) {
      int next_j = 0;
      if (j < num_points_in_circle_ - 1) next_j = j + 1;
      glm::vec3 a = vertices_[1 + i * num_points_in_circle_ + j];
      glm::vec3 b = vertices_[1 + i * num_points_in_circle_ + next_j];
      glm::vec3 c = vertices_[1 + (i + 1) * num_points_in_circle_ + j];
      glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
      normals_.push_back(normal);
    }
  }

  for (int j = 0; j < num_points_in_circle_; j++) {
    normals_.push_back(glm::vec3(0, -1, 0));
  }
  normals_.push_back(glm::vec3(0, -1, 0));
}

void Rock::Smooth() {
  for (int k = 0; k < 2; k++) {
    int dome_radius = 1000.0f;
    for (int i = 0; i < vertices_.size(); i++) {
      float distance = glm::distance(glm::vec3(0.0f), vertices_[i]);
      if (distance > dome_radius / 2) {
        vertices_[i] *= 0.99;
      } else {
        vertices_[i] *= 1.01;
      }
    }
  }
}

void Rock::DrawShit(
  glm::mat4 ModelMatrix, glm::mat4 ProjectionMatrix, 
  glm::mat4 ViewMatrix, glm::vec3 color, int lod
) {
  glUseProgram(shader_.program_id());

  shader_.BindTexture("RockTextureSampler", texture_);
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
  glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader_.GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

  glUniform3fv(shader_.GetUniformId("rock_color"), 1, (float*) &rock_color_);

  shader_.BindBuffer(vertex_buffer_, 0, 3);
  shader_.BindBuffer(uv_buffer_, 1, 2);
  shader_.BindBuffer(normal_buffer_, 2, 3);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffers_[lod]);
  glDrawElements(GL_TRIANGLES, sizes_[lod], GL_UNSIGNED_INT, (void*) 0);

  shader_.Clear();
}

void Rock::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  static int counter = 0;
  glUseProgram(shader_.program_id());

  shader_.BindTexture("RockTextureSampler", texture_);
  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
  glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader_.GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

  glUniform3fv(shader_.GetUniformId("rock_color"), 1, (float*) &rock_color_);

  shader_.BindBuffer(vertex_buffer_, 0, 3);
  shader_.BindBuffer(uv_buffer_, 1, 2);
  shader_.BindBuffer(normal_buffer_, 2, 3);

  static int lod_level = 0;
  counter++;
  if (counter % 480 == 0) lod_level++;
  if (lod_level > 4) lod_level = 0;
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffers_[lod_level]);
  glDrawElements(GL_TRIANGLES, sizes_[lod_level], GL_UNSIGNED_INT, (void*) 0);

  shader_.Clear();
}

} // End of namespace.
