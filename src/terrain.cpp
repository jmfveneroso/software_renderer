#include "terrain.hpp"

namespace Sibyl {

float Clipmap::GetHeight(float x, float y) {
  return noise_.noise(x * 0.001, y * 0.001);
}

Clipmap::Clipmap() {}

Clipmap::Clipmap(unsigned int level) 
  : level_(level) {
  Init();
}

unsigned int Clipmap::GetTileSize() {
  return 1 << (level_ - 1);
}

void Clipmap::Init() {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &height_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);
  glGenBuffers(1, &barycentric_buffer_);

  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> barycentric;

  for (int z = 0; z <= CLIPMAP_SIZE; z++) {
    for (int x = 0; x <= CLIPMAP_SIZE; x++) {
      vertices_[z * (CLIPMAP_SIZE + 1) + x] = glm::vec3(x, 0, z);
      height_[z * (CLIPMAP_SIZE + 1) + x] = 0;
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
  glBufferData(GL_ARRAY_BUFFER, (CLIPMAP_SIZE + 1) * (CLIPMAP_SIZE + 1) * sizeof(glm::vec3), &vertices_, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, height_buffer_);
  glBufferData(GL_ARRAY_BUFFER, (CLIPMAP_SIZE + 1) * (CLIPMAP_SIZE + 1) * sizeof(glm::vec3), &height_, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, barycentric_buffer_);
  glBufferData(GL_ARRAY_BUFFER, barycentric.size() * sizeof(glm::vec3), &barycentric[0], GL_STATIC_DRAW);


  // Breaking heart.
  for (int z = 0; z < CLIPMAP_SIZE+1; z++) {
    for (int x = 0; x < CLIPMAP_SIZE+1; x++) {
      height_map_[z * (CLIPMAP_SIZE+1) + x] = float(1 + GetHeight(x * TILE_SIZE, z * TILE_SIZE)) / 2;
      valid_[z * (CLIPMAP_SIZE+1) + x] = true;
    }
  }

  glGenTextures(1, &height_texture_);
  glBindTexture(GL_TEXTURE_2D, height_texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CLIPMAP_SIZE+1, CLIPMAP_SIZE+1, 0, GL_RED, GL_FLOAT, height_map_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

glm::ivec3 Clipmap::GetToroidalCoordinates(glm::ivec3 grid_coordinates) {
  glm::ivec3 clipmap_coords;
  clipmap_coords.x = (grid_coordinates.x - top_left_.x) / int(GetTileSize());
  clipmap_coords.z = (grid_coordinates.z - top_left_.z) / int(GetTileSize());

  glm::ivec3 toroidal_coords;
  toroidal_coords.x = (clipmap_coords.x + CLIPMAP_SIZE + 1 - buffer_top_left_.x) % (CLIPMAP_SIZE + 1);
  toroidal_coords.z = (clipmap_coords.z + CLIPMAP_SIZE + 1 - buffer_top_left_.z) % (CLIPMAP_SIZE + 1);
  return toroidal_coords;
}

void Clipmap::InvalidateAll() {
  for (int z = 0; z <= CLIPMAP_SIZE; z++) {
    for (int x = 0; x <= CLIPMAP_SIZE; x++) {
      valid_[z * (CLIPMAP_SIZE+1) + x] = false;
    }
  }
  buffer_top_left_ = glm::ivec3(0, 0, 0);
}

void Clipmap::InvalidateColumns(glm::ivec3 new_top_left) {
  if (top_left_.x == new_top_left.x) return;

  // Invalidate all columns.
  if (abs((new_top_left.x - top_left_.x) / int(GetTileSize())) > CLIPMAP_SIZE) {
    InvalidateAll();
    return;
  }

  glm::ivec3 toroidal_coords;
  int x = top_left_.x;
  int step_x = (new_top_left.x > top_left_.x) ? GetTileSize() : -GetTileSize();
  std::cout << "top_left_.x: " << top_left_.x << std::endl;
  std::cout << "new_top_left.x: " << new_top_left.x << std::endl;
  std::cout << "step_x: " << step_x << std::endl;
  for (; x != new_top_left.x; x += step_x) {
    toroidal_coords = GetToroidalCoordinates(glm::ivec3(x, 0, 0));
   
    std::cout << GetTileSize() << " cut it down: " << toroidal_coords.x << std::endl;
    // Invalidate column. 
    for (int z = 0; z <= CLIPMAP_SIZE; z++) {
      valid_[z * (CLIPMAP_SIZE+1) + toroidal_coords.x] = false;
    }
  }
 
  // buffer_top_left_.x = (buffer_top_left_.x + toroidal_coords.x) % (CLIPMAP_SIZE + 1); 
  buffer_top_left_.x = GetToroidalCoordinates(glm::ivec3(x, 0, 0)).x;
  std::cout << "bla: " << buffer_top_left_.x << std::endl;
}

void Clipmap::InvalidateRows(glm::ivec3 new_top_left) {
  if (top_left_.z == new_top_left.z) return;

  // Invalidate all columns.
  if (abs((new_top_left.z - top_left_.z) / int(GetTileSize())) > CLIPMAP_SIZE) {
    InvalidateAll();
    return;
  }

  glm::ivec3 toroidal_coords;
  int z = top_left_.z;
  int step_z = (new_top_left.z > top_left_.z) ? GetTileSize() : -GetTileSize();
  for (; z != new_top_left.z; z += step_z) {
    toroidal_coords = GetToroidalCoordinates(glm::ivec3(0, 0, z));
   
    // Invalidate column. 
    for (int x = 0; x <= CLIPMAP_SIZE; x++) {
      valid_[toroidal_coords.z * (CLIPMAP_SIZE+1) + x] = false;
    }
  }
 
  buffer_top_left_.z = toroidal_coords.z; 
}

void Clipmap::Invalidate(glm::ivec3 new_top_left) {
  InvalidateColumns(new_top_left);
  // InvalidateRows(new_top_left);
}

void Clipmap::Update(int x, int z) {
  // Offset to top left.
  int offset = ((CLIPMAP_SIZE - 2) / 2);

  glm::ivec3 new_top_left;
  new_top_left.x = (x / (2 * GetTileSize())) * (2 * GetTileSize()) - (offset * GetTileSize());
  new_top_left.z = (z / (2 * GetTileSize())) * (2 * GetTileSize()) - (offset * GetTileSize());
  Invalidate(new_top_left);
  top_left_ = new_top_left;

  for (int z = 0; z <= CLIPMAP_SIZE; z++) {
    for (int x = 0; x <= CLIPMAP_SIZE; x++) {
      int world_coord_x = top_left_.x * TILE_SIZE + x * TILE_SIZE * GetTileSize();
      int world_coord_z = top_left_.z * TILE_SIZE + z * TILE_SIZE * GetTileSize();
      height_[z * (CLIPMAP_SIZE + 1) + x] = 100 * GetHeight(world_coord_x, world_coord_z);

      if (valid_[z * (CLIPMAP_SIZE+1) + x]) {
        height_map_[z * (CLIPMAP_SIZE + 1) + x] = 1;
      } else {
        height_map_[z * (CLIPMAP_SIZE + 1) + x] = 0;
      }
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, height_buffer_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, (CLIPMAP_SIZE + 1) * (CLIPMAP_SIZE + 1) * sizeof(glm::vec3), height_);

  glBindTexture(GL_TEXTURE_2D, height_texture_);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (CLIPMAP_SIZE + 1), (CLIPMAP_SIZE + 1), GL_RED, GL_FLOAT, height_map_);
}

void Clipmap::DrawSubRegion(int start_x, int end_x, int start_z, int end_z) {
  indices_.clear();

  for (unsigned int z = start_z; z < end_z; z++) {
    if (z > start_z) indices_.push_back(z * (CLIPMAP_SIZE + 1) + start_x);
    for (unsigned int x = start_x; x <= end_x; x++) {
      indices_.push_back(z * (CLIPMAP_SIZE + 1) + x);
      indices_.push_back((z + 1) * (CLIPMAP_SIZE + 1) + x);
    } 
    if (z < end_z - 1) indices_.push_back((z + 1) * (CLIPMAP_SIZE + 1) + end_x);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);
  glDrawElements(GL_TRIANGLE_STRIP, indices_.size(), GL_UNSIGNED_INT, (void*) 0);

  // buffer_top_left_ = glm::vec3(0, 0, 0);
}

void Clipmap::Render(
  glm::vec3 player_pos, Shader* shader, glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix,
  glm::ivec3 next_top_left,
  glm::ivec3 next_bottom_right
) {
  shader->BindTexture("HeightMapSampler", height_texture_);

  // Clipmap grid clamp.
  glUniform1i(shader->GetUniformId("TILE_SIZE"), TILE_SIZE * GetTileSize());
  glUniform1i(shader->GetUniformId("CLIPMAP_SIZE"), CLIPMAP_SIZE);

  int x = (int) player_pos.x / TILE_SIZE;
  int z = (int) player_pos.z / TILE_SIZE;
  Update(x, z);

  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(top_left_.x * TILE_SIZE, 0, top_left_.z * TILE_SIZE));
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  glUniform3iv(shader->GetUniformId("buffer_top_left"), 1, (int*) &buffer_top_left_);
  std::cout << "buffer.x: " << buffer_top_left_.x << std::endl;

  glUniformMatrix4fv(shader->GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader->GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader->GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader->GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

  unsigned int size_x = CLIPMAP_SIZE - 2;
  unsigned int size_z = CLIPMAP_SIZE - 2;
  if (x % (2 * GetTileSize()) != 0) size_x = CLIPMAP_SIZE;
  if (z % (2 * GetTileSize()) != 0) size_z = CLIPMAP_SIZE;

  bottom_right_.x = top_left_.x + size_x * GetTileSize();
  bottom_right_.z = top_left_.z + size_z * GetTileSize();

  shader->BindBuffer(vertex_buffer_, 0, 3);
  shader->BindBuffer(uv_buffer_, 1, 2);
  shader->BindBuffer(barycentric_buffer_, 2, 3);
  shader->BindBuffer(height_buffer_, 3, 1);

  if (level_ == 1) {
    DrawSubRegion(0, size_x, 0, size_z);
  } else {
    // Region 1 (LEFT).
    int start_x = 0;
    int end_x = (next_top_left.x - top_left_.x) / GetTileSize();
    int start_z = 0;
    int end_z = size_z;
    DrawSubRegion(start_x, end_x, start_z, end_z);

    // Region 2 (UP).
    start_x = (next_top_left.x - top_left_.x) / GetTileSize();
    end_x = ((next_bottom_right.x - top_left_.x) / GetTileSize());
    start_z = 0;
    end_z = (next_top_left.z - top_left_.z) / GetTileSize();
    DrawSubRegion(start_x, end_x, start_z, end_z);

    // Region 3 (DOWN).
    start_x = (next_top_left.x - top_left_.x) / GetTileSize();
    end_x = ((next_bottom_right.x - top_left_.x) / GetTileSize());
    start_z = ((next_bottom_right.z - top_left_.z) / GetTileSize());
    end_z = size_z;
    DrawSubRegion(start_x, end_x, start_z, end_z);

    // Region 4 (RIGHT).
    start_x = ((next_bottom_right.x - top_left_.x) / GetTileSize());
    end_x = size_x;
    start_z = 0;
    end_z = size_z;
    DrawSubRegion(start_x, end_x, start_z, end_z);
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

    glm::ivec3 top_left;
    int offset = ((CLIPMAP_SIZE - 2) / 2);
    top_left.x = -(offset * clipmaps_[i].GetTileSize());
    top_left.z = -(offset * clipmaps_[i].GetTileSize());
    clipmaps_[i].set_top_left(top_left);
  }
}

void Terrain::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());

  // Textures.
  shader_.BindTexture("DiffuseTextureSampler", diffuse_texture_id_);
  shader_.BindTexture("NormalTextureSampler", normal_texture_id_);
  shader_.BindTexture("SpecularTextureSampler", specular_texture_id_);
  // shader_.BindTexture("HeightMapSampler", height_map_);

  // Clipmap.
  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    glm::ivec3 top_left = glm::ivec3(0, 0, 0);
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
