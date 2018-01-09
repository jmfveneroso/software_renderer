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


  glGenBuffers(1, &center_region_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, center_region_buffer_);
  center_region_size_ = CreateRenderRegion(glm::ivec2(0, 0), glm::ivec2(CLIPMAP_SIZE, CLIPMAP_SIZE));

  for (int x = 0; x < 2; x++) {
    for (int y = 0; y < 2; y++) {
      int left_size = CLIPMAP_SIZE / 4 + x;
      int up_size   = CLIPMAP_SIZE / 4 + y;
 
      for (int region = 0; region < 4; region++) {
        glGenBuffers(1, &render_region_buffers_[x][y][region]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_region_buffers_[x][y][region]);
      
        glm::ivec2 top_left;   
        glm::ivec2 size;   
        switch (region) {
          case RR_LEFT:
            top_left = glm::ivec2(0, 0);
            size = glm::ivec2(left_size, CLIPMAP_SIZE);
            break;
          case RR_TOP:
            top_left = glm::ivec2(left_size, 0);
            size = glm::ivec2(CLIPMAP_SIZE/2, up_size);
            break;
          case RR_BOTTOM:
            top_left = glm::ivec2(left_size, up_size + CLIPMAP_SIZE/2);
            size = glm::ivec2(CLIPMAP_SIZE/2, CLIPMAP_SIZE/2 - up_size);
            break;
          case RR_RIGHT:
            top_left = glm::ivec2(left_size + CLIPMAP_SIZE / 2, 0);
            size = glm::ivec2(CLIPMAP_SIZE/2 - left_size, CLIPMAP_SIZE);
            break;
          default: throw;
        }
        render_region_sizes_[x][y][region] = CreateRenderRegion(top_left, size);
      }
    }
  }
}

int Clipmap::CreateRenderRegion(glm::ivec2 top_left, glm::ivec2 size) {
  std::vector<unsigned int> indices;

  int end_x = top_left.x + size.x;
  int end_y = top_left.y + size.y;
  for (int y = top_left.y; y < end_y; y++) {
    if (y > top_left.y)
      indices.push_back(y * (CLIPMAP_SIZE + 1) + top_left.x);

    for (int x = top_left.x; x <= end_x; x++) {
      indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
      indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
    } 

    if (y < end_y - 1)
      indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + end_x);
  }

  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices.size() * sizeof(unsigned int), 
    &indices[0], 
    GL_STATIC_DRAW
  );

  return indices.size();
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
  glm::ivec2 new_top_left = ClampGridCoordinates(grid_coords, GetTileSize()) - CLIPMAP_OFFSET * GetTileSize();
  InvalidateOuterBuffer(new_top_left);
  top_left_ = new_top_left;

  UpdateHeightMap();

  // glBindTexture(GL_TEXTURE_RECTANGLE, valid_texture_);
  // glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, (CLIPMAP_SIZE + 1), (CLIPMAP_SIZE + 1), GL_RED, GL_FLOAT, height_buffer_.valid);
}

void Clipmap::Render(
  glm::vec3 player_pos, 
  Shader* shader, 
  glm::mat4 ProjectionMatrix, 
  glm::mat4 ViewMatrix
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

  if (level_ == 1) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, center_region_buffer_);
    glDrawElements(GL_TRIANGLE_STRIP, center_region_size_, GL_UNSIGNED_INT, (void*) 0);
  } else {
    glm::ivec2 grid_coords = WorldToGridCoordinates(player_pos);
    glm::ivec2 clipmap_offset = ClampGridCoordinates(grid_coords, GetTileSize() >> 1);
    clipmap_offset -= ClampGridCoordinates(grid_coords, GetTileSize());
    clipmap_offset /= GetTileSize();

    for (int region = 0 ; region < 4; region++) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_region_buffers_[clipmap_offset.x][clipmap_offset.y][region]);
      glDrawElements(GL_TRIANGLE_STRIP, render_region_sizes_[clipmap_offset.x][clipmap_offset.y][region], GL_UNSIGNED_INT, (void*) 0);
    }
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
    clipmaps_[i].Render(player_->position(), &shader_, ProjectionMatrix, ViewMatrix);
  }

  shader_.Clear();
}

} // End of namespace.
