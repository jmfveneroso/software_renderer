#include "clipmap.hpp"

namespace Sibyl {

std::vector<glm::ivec2> Clipmap::feature_points = std::vector<glm::ivec2>();

Clipmap::Clipmap() {}

Clipmap::Clipmap(
  std::shared_ptr<Player> player,
  std::shared_ptr<HeightMap> height_map,
  unsigned int level
) : player_(player), height_map_(height_map), level_(level) {
  Init();
}

int Clipmap::CreateSubRegion(glm::ivec2 top_left, glm::ivec2 size, short subregion_borders) {
  bool borders[4];
  borders[RR_LEFT  ] = subregion_borders & 8;
  borders[RR_TOP   ] = subregion_borders & 4;
  borders[RR_BOTTOM] = subregion_borders & 2;
  borders[RR_RIGHT ] = subregion_borders & 1;

  std::vector<unsigned int> indices;
  for (int y = top_left.y; y < top_left.y + size.y; y++) {
    for (int x = top_left.x; x < top_left.x + size.x; x++) {
      if (borders[RR_TOP] && y == 0) {
        if (x % 2 == 0) {
          if (x == 0 && borders[RR_LEFT]) {
            indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
            indices.push_back((y + 2) * (CLIPMAP_SIZE + 1) + x);
            indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
          } else {
            indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
            indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
            indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
          }

          indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
          indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
          indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 2));
        } else {
          if (x == CLIPMAP_SIZE - 1 && borders[RR_RIGHT]) {
            indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
            indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
            indices.push_back((y + 2) * (CLIPMAP_SIZE + 1) + (x + 1));
          } else {
            indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
            indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
            indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
          }
        }
        continue;
      }

      if (borders[RR_BOTTOM] && y == CLIPMAP_SIZE - 1) {
        if (x % 2 == 0) {
          if (!borders[RR_LEFT] || x != 0) {
            indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
            indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
            indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
          }
          indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
          indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
          indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 2));
        } else {
          if (borders[RR_RIGHT] && x == CLIPMAP_SIZE - 1) continue;
          indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
          indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
          indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
        }
        continue;
      }

      if (borders[RR_LEFT] && x == 0) {
        if (y % 2 == 0) {
          indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
          indices.push_back((y + 2) * (CLIPMAP_SIZE + 1) + x);
          indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
          indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
          indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
          indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
        } else {
          indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
          indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
          indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
        }
        continue;
      }

      if (borders[RR_RIGHT] && x == CLIPMAP_SIZE - 1) {
        if (y % 2 == 0) {
          indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
          indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
          indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
          indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
          indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
          indices.push_back((y + 2) * (CLIPMAP_SIZE + 1) + (x + 1));
        } else {
          indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
          indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
          indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
        }
        continue;
      }

      if ((y % 2 == 0 && x % 2 == 0) || (y % 2 == 1 && x % 2 == 1)) {
        indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
        indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
        indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
        indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
        indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
        indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
      } else {
        indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
        indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
        indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
        indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
        indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
        indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
      }
    } 
  }

  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices.size() * sizeof(unsigned int), 
    &indices[0], 
    GL_STATIC_DRAW
  );

  return indices.size();
}

void Clipmap::CreateSubRegions() {
  glGenBuffers(1, &center_region_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, center_region_buffer_);
  center_region_size_ = CreateSubRegion(glm::ivec2(0, 0), glm::ivec2(CLIPMAP_SIZE, CLIPMAP_SIZE), CENTER_BORDERS);

  for (int x = 0; x < 2; x++) {
    for (int y = 0; y < 2; y++) {
      int left_size = CLIPMAP_SIZE / 4 + x;
      int up_size   = CLIPMAP_SIZE / 4 + y;
 
      for (int region = 0; region < 4; region++) {
        glGenBuffers(1, &render_region_buffers_[x][y][region]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_region_buffers_[x][y][region]);
     
        short borders = 0; 
        glm::ivec2 top_left;   
        glm::ivec2 size;   
        switch (region) {
          case RR_LEFT:
            borders = LEFT_BORDERS;
            top_left = glm::ivec2(0, 0);
            size = glm::ivec2(left_size, CLIPMAP_SIZE);
            break;
          case RR_TOP:
            borders = TOP_BORDERS;
            top_left = glm::ivec2(left_size, 0);
            size = glm::ivec2(CLIPMAP_SIZE/2, up_size);
            break;
          case RR_BOTTOM:
            borders = BOTTOM_BORDERS;
            top_left = glm::ivec2(left_size, up_size + CLIPMAP_SIZE/2);
            size = glm::ivec2(CLIPMAP_SIZE/2, CLIPMAP_SIZE/2 - up_size);
            break;
          case RR_RIGHT:
            borders = RIGHT_BORDERS;
            top_left = glm::ivec2(left_size + CLIPMAP_SIZE / 2, 0);
            size = glm::ivec2(CLIPMAP_SIZE/2 - left_size, CLIPMAP_SIZE);
            break;
          default: throw;
        }
        render_region_top_left_[x][y][region] = top_left;
        render_region_clip_size_[x][y][region] = size;
        render_region_sizes_[x][y][region] = CreateSubRegion(top_left, size, borders);
      }
    }
  }
}

void Clipmap::Init() {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);
  glGenBuffers(1, &barycentric_buffer_);

  std::vector<glm::vec2> uvs;
  std::vector<glm::vec4> barycentric;

  for (int z = 0; z <= CLIPMAP_SIZE; z++) {
    for (int x = 0; x <= CLIPMAP_SIZE; x++) {
      vertices_[z * (CLIPMAP_SIZE + 1) + x] = glm::vec3(x, 0, z);
      uvs.push_back(glm::vec2(x * GetTileSize(), z * GetTileSize()));

      glm::vec4 b;
      if (z % 3 == 0) {
        if (x % 3 == 0) b = glm::vec4(1, 0, 0, 0);
        if (x % 3 == 1) b = glm::vec4(0, 1, 0, 0);
        if (x % 3 == 2) b = glm::vec4(0, 0, 1, 0);
      } else if (z % 3 == 1) {
        if (x % 3 == 0) b = glm::vec4(0, 0, 1, 0);
        if (x % 3 == 1) b = glm::vec4(1, 0, 0, 0);
        if (x % 3 == 2) b = glm::vec4(0, 1, 0, 0);
      } else {
        if (x % 3 == 0) b = glm::vec4(0, 1, 0, 0);
        if (x % 3 == 1) b = glm::vec4(0, 0, 1, 0);
        if (x % 3 == 2) b = glm::vec4(1, 0, 0, 0);
      }
      barycentric.push_back(b);
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, (CLIPMAP_SIZE + 1) * (CLIPMAP_SIZE + 1) * sizeof(glm::vec3), vertices_, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, barycentric_buffer_);
  glBufferData(GL_ARRAY_BUFFER, barycentric.size() * sizeof(glm::vec4), &barycentric[0], GL_STATIC_DRAW);

  for (int z = 0; z < CLIPMAP_SIZE+1; z++) {
    for (int x = 0; x < CLIPMAP_SIZE+1; x++) {
      height_buffer_.valid[z * (CLIPMAP_SIZE+1) + x] = 0;
    }
  }

  for (int i = 0; i < 2; i++) {
    glGenTextures(1, &height_texture_[i]);
    glBindTexture(GL_TEXTURE_RECTANGLE, height_texture_[i]);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RED, CLIPMAP_SIZE+1, CLIPMAP_SIZE+1, 0, GL_RED, GL_FLOAT, height_buffer_.height);

    glGenTextures(1, &normals_texture_[i]);
    glBindTexture(GL_TEXTURE_RECTANGLE, normals_texture_[i]);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, CLIPMAP_SIZE+1, CLIPMAP_SIZE+1, 0, GL_RGB, GL_FLOAT, height_buffer_.normals);
  }

  CreateSubRegions();

  feature_points.reserve(10);
  for (int i = 0; i < 10; i++) {
    int x = -500 * TILE_SIZE + rand() % (1000 * TILE_SIZE);
    int y = -500 * TILE_SIZE + rand() % (1000 * TILE_SIZE); 
    feature_points[i] = glm::ivec2(x, y);
  }
}

void Clipmap::Clear() {
  num_invalid_ = 0;
  for (int z = 0; z < CLIPMAP_SIZE+1; z++) {
    for (int x = 0; x < CLIPMAP_SIZE+1; x++) {
      height_buffer_.valid[z * (CLIPMAP_SIZE+1) + x] = 0;
      num_invalid_++;
    }
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
      // Invalidate column.
      for (int y = 0; y < CLIPMAP_SIZE + 1; y++) {
        if (height_buffer_.valid[y * (CLIPMAP_SIZE + 1) + x]) {
          height_buffer_.valid[y * (CLIPMAP_SIZE + 1) + x] = 0;
          num_invalid_++;
        }
      }
    }
  }

  // Rows.
  for (int y = 0; y < CLIPMAP_SIZE + 1; y++) {
    glm::ivec2 grid_coords = BufferToGridCoordinates(glm::ivec2(0, y));
    if (grid_coords.y < new_top_left.y || grid_coords.y > new_bottom_right.y) {
      // Invalidate row.
      for (int x = 0; x < CLIPMAP_SIZE + 1; x++) {
        if (height_buffer_.valid[y * (CLIPMAP_SIZE + 1) + x]) {
          height_buffer_.valid[y * (CLIPMAP_SIZE + 1) + x] = 0;
          num_invalid_++;
        }
      }
    }
  }
  
  height_buffer_.top_left = GridToBufferCoordinates(new_top_left);
}

void Clipmap::Update(glm::vec3 player_pos) {
  glm::ivec2 grid_coords = WorldToGridCoordinates(player_pos);
  glm::ivec2 new_top_left = ClampGridCoordinates(grid_coords, GetTileSize()) - CLIPMAP_OFFSET * GetTileSize();

  InvalidateOuterBuffer(new_top_left);
  if (top_left_ == new_top_left && num_invalid_ == 0) return;
  top_left_ = new_top_left;

  if (num_invalid_ <= 0) return;

  for (int y = 0; y < CLIPMAP_SIZE + 1; y++) {
    if (num_invalid_ <= 0) break;
    for (int x = 0; x < CLIPMAP_SIZE + 1; x++) {
      if (num_invalid_ <= 0) break;
      if (height_buffer_.valid[y * (CLIPMAP_SIZE+1) + x]) continue;

      glm::ivec2 grid_coords = BufferToGridCoordinates(glm::ivec2(x, y));
      glm::vec3 world_coords = GridToWorldCoordinates(grid_coords);

      height_buffer_.height[y * (CLIPMAP_SIZE+1) + x] = float(1 + height_map_->GetHeight(world_coords.x, world_coords.z)) / 2;
      height_buffer_.valid[y * (CLIPMAP_SIZE+1) + x] = 1;

      float step = GetTileSize() * TILE_SIZE;
      // glm::vec3 a = glm::vec3(0,    8000 * (float(1 + height_map_->GetHeight(world_coords.x       , world_coords.z        )) / 2), 0);
      // glm::vec3 b = glm::vec3(step, 8000 * (float(1 + height_map_->GetHeight(world_coords.x + step, world_coords.z        )) / 2), 0);
      // glm::vec3 c = glm::vec3(0,    8000 * (float(1 + height_map_->GetHeight(world_coords.x       , world_coords.z + step )) / 2), step);
      glm::vec3 a = glm::vec3(0,    MAX_HEIGHT * (float(1 + height_map_->GetHeight(world_coords.x       , world_coords.z        )) / 2), 0);
      glm::vec3 b = glm::vec3(step, MAX_HEIGHT * (float(1 + height_map_->GetHeight(world_coords.x + step, world_coords.z        )) / 2), 0);
      glm::vec3 c = glm::vec3(0,    MAX_HEIGHT * (float(1 + height_map_->GetHeight(world_coords.x       , world_coords.z + step )) / 2), step);
      height_buffer_.normals[y * (CLIPMAP_SIZE+1) + x] = (normalize(glm::cross(c - a, b - a)) + 1.0f) / 2.0f;
      num_invalid_--;
    }
  }
 
  if (++active_texture_ > 1) active_texture_ = 0;
  glBindTexture(GL_TEXTURE_RECTANGLE, height_texture_[active_texture_]);
  glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, CLIPMAP_SIZE + 1, CLIPMAP_SIZE + 1, GL_RED, GL_FLOAT, &height_buffer_.height[0]);
  glBindTexture(GL_TEXTURE_RECTANGLE, normals_texture_[active_texture_]);
  glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, CLIPMAP_SIZE + 1, CLIPMAP_SIZE + 1, GL_RGB, GL_FLOAT, &height_buffer_.normals[0]);
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
  glUniform1i(shader->GetUniformId("TILE_SIZE"), TILE_SIZE * GetTileSize());
  glUniform1i(shader->GetUniformId("CLIPMAP_SIZE"), CLIPMAP_SIZE);
  glUniform1i(shader->GetUniformId("MAX_HEIGHT"), MAX_HEIGHT);
  glUniform2iv(shader->GetUniformId("buffer_top_left"), 1, (int*) &height_buffer_.top_left);

  shader->BindBuffer(vertex_buffer_, 0, 3);
  shader->BindBuffer(uv_buffer_, 1, 2);
  shader->BindBuffer(barycentric_buffer_, 2, 4);

  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_RECTANGLE, normals_texture_[active_texture_]);
  glUniform1i(shader->GetUniformId("NormalsSampler"), 7);

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_RECTANGLE, height_texture_[active_texture_]);
  glUniform1i(shader->GetUniformId("HeightMapSampler"), 6);

  if (center) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, center_region_buffer_);
    glDrawElements(GL_TRIANGLES, center_region_size_, GL_UNSIGNED_INT, (void*) 0);
    return;
  } 

  glm::ivec2 grid_coords = WorldToGridCoordinates(player_pos);
  glm::ivec2 clipmap_offset = ClampGridCoordinates(grid_coords, GetTileSize() >> 1);
  clipmap_offset -= ClampGridCoordinates(grid_coords, GetTileSize());
  clipmap_offset /= GetTileSize();
  for (int region = 0 ; region < 4; region++) {
    glm::vec2 top_lft = top_left_ * TILE_SIZE + render_region_top_left_[clipmap_offset.x][clipmap_offset.y][region] * GetTileSize() * TILE_SIZE;
    glm::vec2 bot_rgt = top_lft + glm::vec2(render_region_clip_size_[clipmap_offset.x][clipmap_offset.y][region] * GetTileSize() * TILE_SIZE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_region_buffers_[clipmap_offset.x][clipmap_offset.y][region]);
    glDrawElements(GL_TRIANGLES, render_region_sizes_[clipmap_offset.x][clipmap_offset.y][region], GL_UNSIGNED_INT, (void*) 0);
  }
} 

void Clipmap::RenderWater(
  glm::vec3 player_pos, 
  Shader* shader, 
  glm::mat4 ProjectionMatrix, 
  glm::mat4 ViewMatrix,
  glm::vec3 camera,
  bool center,
  std::shared_ptr<Water> water
) {
  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(top_left_.x * TILE_SIZE, 0, top_left_.y * TILE_SIZE));
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
  glUniformMatrix4fv(shader->GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader->GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader->GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader->GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
  glUniform1i(shader->GetUniformId("TILE_SIZE"), TILE_SIZE * GetTileSize());
  glUniform1i(shader->GetUniformId("CLIPMAP_SIZE"), CLIPMAP_SIZE);
  glUniform1i(shader->GetUniformId("MAX_HEIGHT"), MAX_HEIGHT);
  glUniform2iv(shader->GetUniformId("buffer_top_left"), 1, (int*) &height_buffer_.top_left);

  glm::vec3 lightPos = glm::vec3(0, 2000, 0);
  glUniform3f(shader->GetUniformId("LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
  glUniform3fv(shader->GetUniformId("cameraPosition"), 1, (float*) &camera);
  glUniform1f(shader->GetUniformId("moveFactor"), Water::move_factor);

  shader->BindBuffer(vertex_buffer_, 0, 3);
  shader->BindBuffer(uv_buffer_, 1, 2);
  shader->BindBuffer(barycentric_buffer_, 2, 4);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_RECTANGLE, height_texture_[active_texture_]);
  glUniform1i(shader->GetUniformId("HeightMapSampler"), 5);

  if (center) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, center_region_buffer_);
    glDrawElements(GL_TRIANGLES, center_region_size_, GL_UNSIGNED_INT, (void*) 0);
    return;
  } 

  glm::ivec2 grid_coords = WorldToGridCoordinates(player_pos);
  glm::ivec2 clipmap_offset = ClampGridCoordinates(grid_coords, GetTileSize() >> 1);
  clipmap_offset -= ClampGridCoordinates(grid_coords, GetTileSize());
  clipmap_offset /= GetTileSize();
  for (int region = 0 ; region < 4; region++) {
    glm::vec2 top_lft = top_left_ * TILE_SIZE + render_region_top_left_[clipmap_offset.x][clipmap_offset.y][region] * GetTileSize() * TILE_SIZE;
    glm::vec2 bot_rgt = top_lft + glm::vec2(render_region_clip_size_[clipmap_offset.x][clipmap_offset.y][region] * GetTileSize() * TILE_SIZE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_region_buffers_[clipmap_offset.x][clipmap_offset.y][region]);
    glDrawElements(GL_TRIANGLES, render_region_sizes_[clipmap_offset.x][clipmap_offset.y][region], GL_UNSIGNED_INT, (void*) 0);
  }
} 




// DEPRECATED.

// When i need to offset look here.
// glDrawElements(GL_TRIANGLE_STRIP, 54, GL_UNSIGNED_INT, (void*) (44 * sizeof(unsigned int)));

// bool Clipmap::LineIntersection(glm::vec2 a[2], glm::vec2 b[2], glm::vec2* intersection) {
//   double numerator   = glm::determinant(glm::mat2(a[1] - a[0], b[1] - b[0]));
//   double denominator = glm::determinant(glm::mat2(a[1] - a[0], b[1] - b[0]));
// 
//   double delta = (numerator / denominator);
//   if (delta < 0) return false;
// 
//   *intersection = b[0] + b[1] * delta;
//   return true;
// }

// bool Clipmap::IsInsideFrustum(glm::vec2 lft, glm::vec2 rgt, glm::vec2 p) {
//   double denominator = glm::determinant(glm::mat2(lft, rgt));
//   double alpha = glm::determinant(glm::mat2(p, rgt)) / denominator;
//   double beta  = glm::determinant(glm::mat2(lft, p)) / denominator;
//   return alpha >= 0 && beta >= 0;
// }
// 
// bool Clipmap::IsSubregionVisible(glm::vec2 top_left, glm::vec2 bottom_right) {
//   glm::vec2 view_direction = glm::vec2(
//     sin(player_->horizontal_angle()),
//     cos(player_->horizontal_angle())
//   );
//   // std::cout << "horizontal: " << player_->horizontal_angle() << std::endl;
//   // std::cout << "viewdirection x: " << view_direction.x << " y:" << view_direction.y << std::endl;
// 
//   glm::vec2 pos = glm::vec2(player_->position().x, player_->position().z);
//   top_left -= pos;
//   bottom_right -= pos;
// 
//   glm::vec2 lft = glm::rotate(view_direction, glm::radians(player_->fov()));
//   glm::vec2 rgt = glm::rotate(view_direction, -glm::radians(player_->fov()));
// 
//   // std::cout << "lft x: "    << lft.x          << " y:" << lft.y          << std::endl;
//   // std::cout << "rgt x: "    << rgt.x          << " y:" << rgt.y          << std::endl;
//   // std::cout << "toplft x: " << top_left.x     << " y:" << top_left.y     << std::endl;
//   // std::cout << "btmrgt x: " << bottom_right.x << " y:" << bottom_right.y << std::endl;
// 
//   if (IsInsideFrustum(lft, rgt, top_left)    ) return true;
//   if (IsInsideFrustum(lft, rgt, bottom_right)) return true;
//   if (IsInsideFrustum(lft, rgt, glm::vec2(top_left.x, bottom_right.y))) return true;
//   if (IsInsideFrustum(lft, rgt, glm::vec2(bottom_right.x, top_left.y))) return true;
//   return false;
// }

} // End of namespace.
