#include "clipmap.hpp"

namespace Sibyl {

Clipmap::Clipmap() {}

Clipmap::Clipmap(
  float* height_map,
  unsigned int level
) : height_map_(height_map), level_(level) {
  Init();
}

void Clipmap::Init() {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);

  std::vector<glm::vec2> uvs;
  for (int z = 0; z <= CLIPMAP_SIZE; z++) {
    for (int x = 0; x <= CLIPMAP_SIZE; x++) {
      vertices_[z * (CLIPMAP_SIZE + 1) + x] = glm::vec3(x, 0, z);
      uvs.push_back(glm::vec2(x * GetTileSize(), z * GetTileSize()));
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, (CLIPMAP_SIZE + 1) * (CLIPMAP_SIZE + 1) * sizeof(glm::vec3), vertices_, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  for (int z = 0; z < CLIPMAP_SIZE+1; z++) {
    height_buffer_.valid_rows[z] = 0;
    height_buffer_.valid_columns[z] = 0;
  }

  glGenTextures(1, &height_texture_);
  glBindTexture(GL_TEXTURE_RECTANGLE, height_texture_);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RED, CLIPMAP_SIZE+1, CLIPMAP_SIZE+1, 0, GL_RED, GL_FLOAT, NULL);

  glGenTextures(1, &normals_texture_);
  glBindTexture(GL_TEXTURE_RECTANGLE, normals_texture_);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, CLIPMAP_SIZE+1, CLIPMAP_SIZE+1, 0, GL_RGB, GL_FLOAT, NULL);

  // Create subregions.
  for (int region = 0; region < 5; region++) {
    subregions_[region] = Subregion(static_cast<SubregionLabel>(region), level_);
  }
}

int Clipmap::GetTileSize() {
  return 1 << (level_ - 1);
}

glm::ivec2 Clipmap::WorldToGridCoordinates(glm::vec3 coords) {
  return glm::ivec2(coords.x, coords.z) / TILE_SIZE;
}

glm::vec3 Clipmap::GridToWorldCoordinates(glm::ivec2 coords) {
  return glm::ivec3(coords.x * TILE_SIZE, 0, coords.y * TILE_SIZE);
}

glm::ivec2 Clipmap::ClampGridCoordinates(glm::ivec2 coords, int tile_size) {
  glm::ivec2 result = (coords / (2 * tile_size)) * (2 * tile_size);
  if (coords.x < 0 && coords.x != result.x) result.x -= 2 * tile_size;
  if (coords.y < 0 && coords.y != result.y) result.y -= 2 * tile_size;
  return result;
}

glm::ivec2 Clipmap::BufferToGridCoordinates(glm::ivec2 coords) {
  if (
    coords.x < 0 || coords.x > CLIPMAP_SIZE + 1 ||
    coords.y < 0 || coords.y > CLIPMAP_SIZE + 1
  ) {
    throw "Error"; 
  }

  glm::ivec2 toroidal_coords = (coords - height_buffer_.top_left + CLIPMAP_SIZE + 1) % (CLIPMAP_SIZE + 1);
  return top_left_ + toroidal_coords * GetTileSize();
}

glm::ivec2 Clipmap::GridToBufferCoordinates(glm::ivec2 coords) {
  glm::ivec2 clipmap_coords = ((coords - top_left_) / GetTileSize()) % (CLIPMAP_SIZE + 1);
  return (clipmap_coords + height_buffer_.top_left + CLIPMAP_SIZE + 1) % (CLIPMAP_SIZE + 1);
}

void Clipmap::InvalidateOuterBuffer(glm::ivec2 new_top_left) {
  glm::ivec2 new_bottom_right = new_top_left + CLIPMAP_SIZE * GetTileSize();

  // Columns.
  for (int x = 0; x < CLIPMAP_SIZE + 1; x++) {
    glm::ivec2 grid_coords = BufferToGridCoordinates(glm::ivec2(x, 0));
    if (grid_coords.x < new_top_left.x || grid_coords.x > new_bottom_right.x) {
      height_buffer_.valid_columns[x] = 0;
    }
  }

  // Rows.
  for (int y = 0; y < CLIPMAP_SIZE + 1; y++) {
    glm::ivec2 grid_coords = BufferToGridCoordinates(glm::ivec2(0, y));
    if (grid_coords.y < new_top_left.y || grid_coords.y > new_bottom_right.y) {
      height_buffer_.valid_rows[y] = 0;
    }
  }
  
  height_buffer_.top_left = GridToBufferCoordinates(new_top_left);
}

float Clipmap::GetGridHeight(float x, float y) {
  double period = 0.000001f;
  double amplitude = 24.0f * pow(E, -0.0000002f * sqrt(x*x + y*y));
  double long_wave = amplitude * (cos(x * period) + cos(y * period));

  double period2 = 0.00004f;
  double amplitude2 = .5f;
  double short_wave = amplitude2 * (cos(x * period2) + cos(y * period2));
  return -16.f + long_wave + short_wave;
}

void Clipmap::UpdatePoint(int x, int y, float* p_height, glm::vec3* p_normal) {
  glm::ivec2 grid_coords = BufferToGridCoordinates(glm::ivec2(x, y));
  glm::vec3 world_coords = GridToWorldCoordinates(grid_coords);

  float height = float(1 + GetGridHeight(world_coords.x, world_coords.z)) / 2;

  float step = GetTileSize() * TILE_SIZE;
  glm::vec3 a = glm::vec3(0,    MAX_HEIGHT * (float(1 + GetGridHeight(world_coords.x       , world_coords.z        )) / 2), 0);
  glm::vec3 b = glm::vec3(step, MAX_HEIGHT * (float(1 + GetGridHeight(world_coords.x + step, world_coords.z        )) / 2), 0);
  glm::vec3 c = glm::vec3(0,    MAX_HEIGHT * (float(1 + GetGridHeight(world_coords.x       , world_coords.z + step )) / 2), step);
  glm::vec3 tangent = b - a;
  glm::vec3 bitangent = c - a;
  glm::vec3 normal = (normalize(glm::cross(bitangent, tangent)) + 1.0f) / 2.0f;
  num_invalid_--;

  *p_height = height;
  *p_normal = normal;
}

void Clipmap::Update(glm::vec3 player_pos) {
  glm::ivec2 grid_coords = WorldToGridCoordinates(player_pos);
  glm::ivec2 new_top_left = ClampGridCoordinates(grid_coords, GetTileSize()) - CLIPMAP_OFFSET * GetTileSize();

  InvalidateOuterBuffer(new_top_left);
  if (top_left_ == new_top_left && num_invalid_ == 0) return;
  top_left_ = new_top_left;

  // Rows.
  for (int y = 0; y < CLIPMAP_SIZE + 1; y++) {
    if (height_buffer_.valid_rows[y]) continue;
    for (int x = 0; x < CLIPMAP_SIZE + 1; x++) {
      UpdatePoint(x, y, &height_buffer_.row_heights[y][x], &height_buffer_.row_normals[y][x]);
    }
    glBindTexture(GL_TEXTURE_RECTANGLE, height_texture_);
    glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, y, CLIPMAP_SIZE + 1, 1, GL_RED, GL_FLOAT, &height_buffer_.row_heights[y][0]);
    glBindTexture(GL_TEXTURE_RECTANGLE, normals_texture_);
    glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, y, CLIPMAP_SIZE + 1, 1, GL_RGB, GL_FLOAT, &height_buffer_.row_normals[y][0]);
    height_buffer_.valid_rows[y] = true;
  }

  // Columns.
  for (int x = 0; x < CLIPMAP_SIZE + 1; x++) {
    if (height_buffer_.valid_columns[x]) continue;
    for (int y = 0; y < CLIPMAP_SIZE + 1; y++) {
      UpdatePoint(x, y, &height_buffer_.column_heights[x][y], &height_buffer_.column_normals[x][y]);
    }
    glBindTexture(GL_TEXTURE_RECTANGLE, height_texture_);
    glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, x, 0, 1, CLIPMAP_SIZE + 1, GL_RED, GL_FLOAT, &height_buffer_.column_heights[x][0]);
    glBindTexture(GL_TEXTURE_RECTANGLE, normals_texture_);
    glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, x, 0, 1, CLIPMAP_SIZE + 1, GL_RGB, GL_FLOAT, &height_buffer_.column_normals[x][0]);
    height_buffer_.valid_columns[x] = true;
  }
}

void Clipmap::Render(
  glm::vec3 player_pos, 
  Shader* shader, 
  glm::mat4 ProjectionMatrix, 
  glm::mat4 ViewMatrix,
  bool center
) {
  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(top_left_.x * TILE_SIZE, 0, top_left_.y * TILE_SIZE));
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
  glUniformMatrix4fv(shader->GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader->GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader->GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader->GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
  glUniform1i(shader->GetUniformId("PURE_TILE_SIZE"), TILE_SIZE);
  glUniform1i(shader->GetUniformId("TILE_SIZE"), TILE_SIZE * GetTileSize());
  glUniform1i(shader->GetUniformId("CLIPMAP_SIZE"), CLIPMAP_SIZE);
  glUniform1i(shader->GetUniformId("MAX_HEIGHT"), MAX_HEIGHT);
  glUniform2iv(shader->GetUniformId("buffer_top_left"), 1, (int*) &height_buffer_.top_left);
  glUniform2iv(shader->GetUniformId("top_left"), 1, (int*) &top_left_);

  shader->BindBuffer(vertex_buffer_, 0, 3);
  shader->BindBuffer(uv_buffer_, 1, 2);

  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_RECTANGLE, height_texture_);
  glUniform1i(shader->GetUniformId("HeightMapSampler"), 7);

  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_RECTANGLE, normals_texture_);
  glUniform1i(shader->GetUniformId("NormalsSampler"), 8);

  glActiveTexture(GL_TEXTURE9);
  glBindTexture(GL_TEXTURE_RECTANGLE, tangents_texture_);
  glUniform1i(shader->GetUniformId("TangentSampler"), 9);

  glActiveTexture(GL_TEXTURE10);
  glBindTexture(GL_TEXTURE_RECTANGLE, bitangents_texture_);
  glUniform1i(shader->GetUniformId("BitangentSampler"), 10);

  glm::ivec2 clipmap_offset = glm::ivec2(0, 0);
  if (!center) {
    glm::ivec2 grid_coords = WorldToGridCoordinates(player_pos);
    clipmap_offset = ClampGridCoordinates(grid_coords, GetTileSize() >> 1);
    clipmap_offset -= ClampGridCoordinates(grid_coords, GetTileSize());
    clipmap_offset /= GetTileSize();
  }

  for (int region = 0 ; region < 5; region++) {
    if (!center && region == 4) continue;
    subregions_[region].Draw(clipmap_offset, top_left_, false);
  }
} 

void Clipmap::RenderWater(
  glm::vec3 player_pos, 
  Shader* shader, 
  glm::mat4 ProjectionMatrix, 
  glm::mat4 ViewMatrix,
  glm::vec3 camera,
  bool center
) {
  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(top_left_.x * TILE_SIZE, 0, top_left_.y * TILE_SIZE));
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
  glUniformMatrix4fv(shader->GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader->GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader->GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader->GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
  glUniform1i(shader->GetUniformId("PURE_TILE_SIZE"), TILE_SIZE);
  glUniform1i(shader->GetUniformId("TILE_SIZE"), TILE_SIZE * GetTileSize());
  glUniform1i(shader->GetUniformId("CLIPMAP_SIZE"), CLIPMAP_SIZE);
  glUniform1i(shader->GetUniformId("MAX_HEIGHT"), MAX_HEIGHT);
  glUniform2iv(shader->GetUniformId("buffer_top_left"), 1, (int*) &height_buffer_.top_left);
  glUniform2iv(shader->GetUniformId("top_left"), 1, (int*) &top_left_);

  glm::vec3 lightPos = glm::vec3(0, 20000, 0);
  glUniform3f(shader->GetUniformId("LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
  glUniform3fv(shader->GetUniformId("cameraPosition"), 1, (float*) &camera);

  static float water_move_factor = 0;
  water_move_factor += 0.0001f;
  glUniform1f(shader->GetUniformId("moveFactor"), water_move_factor);

  shader->BindBuffer(vertex_buffer_, 0, 3);
  shader->BindBuffer(uv_buffer_, 1, 2);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_RECTANGLE, height_texture_);
  glUniform1i(shader->GetUniformId("HeightMapSampler"), 5);

  glm::ivec2 clipmap_offset = glm::ivec2(0, 0);
  if (!center) {
    glm::ivec2 grid_coords = WorldToGridCoordinates(player_pos);
    clipmap_offset = ClampGridCoordinates(grid_coords, GetTileSize() >> 1);
    clipmap_offset -= ClampGridCoordinates(grid_coords, GetTileSize());
    clipmap_offset /= GetTileSize();
  }

  for (int region = 0 ; region < 5; region++) {
    if (!center && region == 4) continue;
    subregions_[region].Draw(clipmap_offset, top_left_, true);
  }
} 

} // End of namespace.
