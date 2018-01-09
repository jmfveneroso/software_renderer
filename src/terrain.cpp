#include "terrain.hpp"

namespace Sibyl {

Clipmap::Clipmap() {}

Clipmap::Clipmap(unsigned int level) 
  : level_(level) {
  Init();
}

void Clipmap::Init() {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);
  glGenBuffers(1, &barycentric_buffer_);
  glGenBuffers(5, subregion_buffers_);

  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> barycentric;

  for (int z = 0; z <= CLIPMAP_SIZE; z++) {
    for (int x = 0; x <= CLIPMAP_SIZE; x++) {
      vertices_[z * (CLIPMAP_SIZE + 1) + x] = glm::vec3(x, 0, z);
      uvs.push_back(glm::vec2(x * GetTileSize(), z * GetTileSize()));

      glm::vec3 b;
      if (z % 3 == 0) {
        if (x % 3 == 0) b = glm::vec3(1, 0, 0);
        if (x % 3 == 1) b = glm::vec3(0, 1, 0);
        if (x % 3 == 2) b = glm::vec3(0, 0, 1);
      } else if (z % 3 == 1) {
        if (x % 3 == 0) b = glm::vec3(0, 0, 1);
        if (x % 3 == 1) b = glm::vec3(1, 0, 0);
        if (x % 3 == 2) b = glm::vec3(0, 1, 0);
      } else {
        if (x % 3 == 0) b = glm::vec3(0, 1, 0);
        if (x % 3 == 1) b = glm::vec3(0, 0, 1);
        if (x % 3 == 2) b = glm::vec3(1, 0, 0);
      }
      barycentric.push_back(b);
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, (CLIPMAP_SIZE + 1) * (CLIPMAP_SIZE + 1) * sizeof(glm::vec3), vertices_, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, barycentric_buffer_);
  glBufferData(GL_ARRAY_BUFFER, barycentric.size() * sizeof(glm::vec3), &barycentric[0], GL_STATIC_DRAW);

  for (int i = 0; i < 5; i++) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subregion_buffers_[i]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1) * sizeof(unsigned int), subregion_indices_[i], GL_STATIC_DRAW);
  }

  for (int z = 0; z < CLIPMAP_SIZE+1; z++) {
    for (int x = 0; x < CLIPMAP_SIZE+1; x++) {
      height_buffer_.valid[z * (CLIPMAP_SIZE+1) + x] = 0;
    }
  }

  glGenTextures(1, &height_texture_);
  glBindTexture(GL_TEXTURE_RECTANGLE, height_texture_);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RED, CLIPMAP_SIZE+1, CLIPMAP_SIZE+1, 0, GL_RED, GL_FLOAT, height_buffer_.height);

  glGenTextures(1, &normals_texture_);
  glBindTexture(GL_TEXTURE_RECTANGLE, normals_texture_);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, CLIPMAP_SIZE+1, CLIPMAP_SIZE+1, 0, GL_RGB, GL_FLOAT, height_buffer_.normals);

  glGenTextures(1, &valid_texture_);
  glBindTexture(GL_TEXTURE_RECTANGLE, valid_texture_);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RED, CLIPMAP_SIZE+1, CLIPMAP_SIZE+1, 0, GL_RED, GL_FLOAT, height_buffer_.valid);
}

// void Clipmap::DrawSubRegion(int start_x, int end_x, int start_z, int end_z, int subregion) {
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
//   indices_.clear();
//   for (unsigned int z = start_z; z < end_z; z++) {
//     if (z > start_z) indices_.push_back(z * (CLIPMAP_SIZE + 1) + start_x);
//     for (unsigned int x = start_x; x <= end_x; x++) {
//       indices_.push_back(z * (CLIPMAP_SIZE + 1) + x);
//       indices_.push_back((z + 1) * (CLIPMAP_SIZE + 1) + x);
//     } 
//     if (z < end_z - 1) indices_.push_back((z + 1) * (CLIPMAP_SIZE + 1) + end_x);
//   }
// 
//   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_DYNAMIC_DRAW);
//   glDrawElements(GL_TRIANGLE_STRIP, indices_.size(), GL_UNSIGNED_INT, (void*) 0);
// }

void Clipmap::DrawSubRegion(int start_x, int end_x, int start_z, int end_z, int subregion) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subregion_buffers_[subregion]);
  if (update_indices_) {
    int counter = 0;
    for (unsigned int z = start_z; z < end_z; z++) {
      if (z > start_z) {
        subregion_indices_[subregion][counter] = z * (CLIPMAP_SIZE + 1) + start_x;
        counter++;
      }
      for (unsigned int x = start_x; x <= end_x; x++) {
        subregion_indices_[subregion][counter] = z * (CLIPMAP_SIZE + 1) + x;
        counter++;
        subregion_indices_[subregion][counter] = (z + 1) * (CLIPMAP_SIZE + 1) + x;
        counter++;
      } 
      if (z < end_z - 1) {
        subregion_indices_[subregion][counter] = (z + 1) * (CLIPMAP_SIZE + 1) + end_x;
        counter++;
      }
    }
    subregion_sizes_[subregion] = counter;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, subregion_sizes_[subregion] * sizeof(unsigned int), subregion_indices_[subregion], GL_DYNAMIC_DRAW);
  }
  glDrawElements(GL_TRIANGLE_STRIP, subregion_sizes_[subregion], GL_UNSIGNED_INT, (void*) 0);
}

int Clipmap::GetTileSize() {
  return 1 << (level_ - 1);
}

float Clipmap::GetHeight(float x, float y) {
  return (3450 * noise_.noise(x * 0.00005, y * 0.00005) +
         510 * noise_.noise(x * 0.0001, y * 0.0001) +
         40 * noise_.noise(x * 0.001, y * 0.001)) / 4000.0f;
}

glm::ivec2 Clipmap::WorldToGridCoordinates(glm::vec3 coords) {
  return glm::ivec2(coords.x, coords.z) / TILE_SIZE;
}

glm::vec3 Clipmap::GridToWorldCoordinates(glm::ivec2 coords) {
  return glm::ivec3(coords.x * TILE_SIZE, 0, coords.y * TILE_SIZE);
}

glm::ivec2 Clipmap::ClampGridCoordinates(glm::ivec2 coords) {
  glm::ivec2 result = (coords / (2 * GetTileSize())) * (2 * GetTileSize());
  if (coords.x < 0 && coords.x != result.x) result.x -= 2 * GetTileSize();
  if (coords.y < 0 && coords.y != result.y) result.y -= 2 * GetTileSize();
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
        height_buffer_.valid[y * (CLIPMAP_SIZE + 1) + x] = 0;
      }
    }
  }

  // Rows.
  for (int y = 0; y < CLIPMAP_SIZE + 1; y++) {
    glm::ivec2 grid_coords = BufferToGridCoordinates(glm::ivec2(0, y));
    if (grid_coords.y < new_top_left.y || grid_coords.y > new_bottom_right.y) {
      // Invalidate row.
      for (int x = 0; x < CLIPMAP_SIZE + 1; x++) {
        height_buffer_.valid[y * (CLIPMAP_SIZE + 1) + x] = 0;
      }
    }
  }
  
  height_buffer_.top_left = GridToBufferCoordinates(new_top_left);
}

void Clipmap::UpdateHeightMap() {
  for (int y = 0; y < CLIPMAP_SIZE + 1; y++) {
    for (int x = 0; x < CLIPMAP_SIZE + 1; x++) {
      glm::ivec2 grid_coords = BufferToGridCoordinates(glm::ivec2(x, y));
      glm::vec3 world_coords = GridToWorldCoordinates(grid_coords);

      if (!height_buffer_.valid[y * (CLIPMAP_SIZE+1) + x]) {
        height_buffer_.height[y * (CLIPMAP_SIZE+1) + x] = float(1 + GetHeight(world_coords.x, world_coords.z)) / 2;
        height_buffer_.valid[y * (CLIPMAP_SIZE+1) + x] = 1;

        float step = GetTileSize() * TILE_SIZE;
        glm::vec3 a = glm::vec3(0,    4000 * (float(1 + GetHeight(world_coords.x       , world_coords.z        )) / 2), 0);
        glm::vec3 b = glm::vec3(step, 4000 * (float(1 + GetHeight(world_coords.x + step, world_coords.z        )) / 2), 0);
        glm::vec3 c = glm::vec3(0,    4000 * (float(1 + GetHeight(world_coords.x       , world_coords.z + step )) / 2), step);
        height_buffer_.normals[y * (CLIPMAP_SIZE+1) + x] = (normalize(glm::cross(c - a, b - a)) + 1.0f) / 2.0f;

        glBindTexture(GL_TEXTURE_RECTANGLE, height_texture_);
        glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, x, y, 1, 1, GL_RED, GL_FLOAT, &height_buffer_.height[y * (CLIPMAP_SIZE+1) + x]);

        glBindTexture(GL_TEXTURE_RECTANGLE, normals_texture_);
        glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, x, y, 1, 1, GL_RGB, GL_FLOAT, &height_buffer_.normals[y * (CLIPMAP_SIZE+1) + x]);
      }
    }
  }
}

void Clipmap::Update(glm::vec3 player_pos) {
  glm::ivec2 grid_coords = WorldToGridCoordinates(player_pos);
  glm::ivec2 new_top_left = ClampGridCoordinates(grid_coords) - CLIPMAP_OFFSET * GetTileSize();
  InvalidateOuterBuffer(new_top_left);
  top_left_ = new_top_left;

  UpdateHeightMap();

  // Update boundaries.
  clipmap_size_.x = CLIPMAP_SIZE - ((grid_coords.x % (2 * GetTileSize()) == 0) ? 2 : 0);
  clipmap_size_.y = CLIPMAP_SIZE - ((grid_coords.y % (2 * GetTileSize()) == 0) ? 2 : 0);
  bottom_right_.x = top_left_.x + clipmap_size_.x * GetTileSize();
  bottom_right_.z = top_left_.y + clipmap_size_.y * GetTileSize();

  // glBindTexture(GL_TEXTURE_RECTANGLE, valid_texture_);
  // glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, (CLIPMAP_SIZE + 1), (CLIPMAP_SIZE + 1), GL_RED, GL_FLOAT, height_buffer_.valid);
}

void Clipmap::Render(
  glm::vec3 player_pos, 
  Shader* shader, 
  glm::mat4 ProjectionMatrix, 
  glm::mat4 ViewMatrix,
  glm::ivec2 next_top_left,
  glm::ivec3 next_bottom_right
) {
  Update(player_pos);



  // Draw.
  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(top_left_.x * TILE_SIZE, 0, top_left_.y * TILE_SIZE));
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
  glUniformMatrix4fv(shader->GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader->GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader->GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader->GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
  glUniform2iv(shader->GetUniformId("buffer_top_left"), 1, (int*) &height_buffer_.top_left);
  glUniform1i(shader->GetUniformId("TILE_SIZE"), TILE_SIZE * GetTileSize());
  glUniform1i(shader->GetUniformId("CLIPMAP_SIZE"), CLIPMAP_SIZE);

  shader->BindBuffer(vertex_buffer_, 0, 3);
  shader->BindBuffer(uv_buffer_, 1, 2);
  shader->BindBuffer(barycentric_buffer_, 2, 3);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_RECTANGLE, normals_texture_);
  glUniform1i(shader->GetUniformId("NormalsSampler"), 5);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_RECTANGLE, height_texture_);
  glUniform1i(shader->GetUniformId("HeightMapSampler"), 4);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_RECTANGLE, valid_texture_);
  glUniform1i(shader->GetUniformId("ValidSampler"), 3);

  // update_indices_ = (counter_++ % 5 == 1);
  update_indices_ = true;

  unsigned int size_x = clipmap_size_.x;
  unsigned int size_z = clipmap_size_.y;
  if (level_ == 1) {
    DrawSubRegion(0, size_x, 0, size_z, 0);
  } else {
    // Region 1 (LEFT).
    int start_x = 0;
    int end_x = (next_top_left.x - top_left_.x) / GetTileSize();
    int start_z = 0;
    int end_z = size_z;
    DrawSubRegion(start_x, end_x, start_z, end_z, 1);

    // Region 2 (UP).
    start_x = (next_top_left.x - top_left_.x) / GetTileSize();
    end_x = ((next_bottom_right.x - top_left_.x) / GetTileSize());
    start_z = 0;
    end_z = (next_top_left.y - top_left_.y) / GetTileSize();
    DrawSubRegion(start_x, end_x, start_z, end_z, 2);

    // Region 3 (DOWN).
    start_x = (next_top_left.x - top_left_.x) / GetTileSize();
    end_x = ((next_bottom_right.x - top_left_.x) / GetTileSize());
    start_z = ((next_bottom_right.z - top_left_.y) / GetTileSize());
    end_z = size_z;
    DrawSubRegion(start_x, end_x, start_z, end_z, 3);

    // Region 4 (RIGHT).
    start_x = ((next_bottom_right.x - top_left_.x) / GetTileSize());
    end_x = size_x;
    start_z = 0;
    end_z = size_z;
    DrawSubRegion(start_x, end_x, start_z, end_z, 4);
  }
} 

Terrain::Terrain(
  std::shared_ptr<Player> player,
  Shader shader, 
  GLuint diffuse_texture_id, 
  GLuint normal_texture_id, 
  GLuint specular_texture_id
) : player_(player),
    shader_(shader),
    diffuse_texture_id_(diffuse_texture_id), 
    normal_texture_id_(normal_texture_id), 
    specular_texture_id_(specular_texture_id) {

  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i] = Clipmap(i + 1); 

    glm::ivec2 top_left;
    top_left.x = -(CLIPMAP_OFFSET * clipmaps_[i].GetTileSize());
    top_left.y = -(CLIPMAP_OFFSET * clipmaps_[i].GetTileSize());
    clipmaps_[i].set_top_left(top_left);
  }
}

void Terrain::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());

  // Textures.
  shader_.BindTexture("DiffuseTextureSampler", diffuse_texture_id_);
  shader_.BindTexture("NormalTextureSampler", normal_texture_id_);
  shader_.BindTexture("SpecularTextureSampler", specular_texture_id_);

  // Clipmap.
  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    glm::ivec2 top_left = glm::ivec2(0, 0);
    glm::ivec3 bottom_right = glm::ivec3(0, 0, 0);

    if (i > 0) {
      top_left = clipmaps_[i - 1].top_left();
      bottom_right = clipmaps_[i - 1].bottom_right();
    }
    clipmaps_[i].Render(player_->position(), &shader_, ProjectionMatrix, ViewMatrix, top_left, bottom_right);
  }

  shader_.Clear();
}

} // End of namespace.
