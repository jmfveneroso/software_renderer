#include "terrain.hpp"

namespace Sibyl {

static int update_counter_ = 0;

std::vector<glm::ivec2> Clipmap::feature_points = std::vector<glm::ivec2>();

Clipmap::Clipmap() {}

Clipmap::Clipmap(
  std::shared_ptr<Player> player,
  unsigned int level
) : player_(player), level_(level) {
  Init();
  QuadIntersection();
}

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

void Clipmap::CreateVoronoiDiagram() {
  feature_points.reserve(10);

  for (int i = 0; i < 10; i++) {
    int x = -500 * TILE_SIZE + rand() % (1000 * TILE_SIZE);
    int y = -500 * TILE_SIZE + rand() % (1000 * TILE_SIZE); 
    feature_points[i] = glm::ivec2(x, y);
    std::cout << "x: " << x << " y: " << y << std::endl;
  }
}

void Clipmap::CreateHeightmap() {
  CreateVoronoiDiagram();
  // for (int x = 0; x < 1000; x++) {
  //   for (int y = 0; y < 1000; y++) {
  //     height_map_[x * 1000 + y] = GetHeight(x * TILE_SIZE, y * TILE_SIZE);
  //   }
  // }
}

bool Clipmap::IsInsideFrustum(glm::vec2 lft, glm::vec2 rgt, glm::vec2 p) {
  double denominator = glm::determinant(glm::mat2(lft, rgt));
  double alpha = glm::determinant(glm::mat2(p, rgt)) / denominator;
  double beta  = glm::determinant(glm::mat2(lft, p)) / denominator;
  // std::cout << "alpha: " << alpha << " beta:" << beta << std::endl;
  return alpha >= 0 && beta >= 0;
}

bool Clipmap::IsSubregionVisible(glm::vec2 top_left, glm::vec2 bottom_right) {
  glm::vec2 view_direction = glm::vec2(
    sin(player_->horizontal_angle()),
    cos(player_->horizontal_angle())
  );
  // std::cout << "horizontal: " << player_->horizontal_angle() << std::endl;
  // std::cout << "viewdirection x: " << view_direction.x << " y:" << view_direction.y << std::endl;

  glm::vec2 pos = glm::vec2(player_->position().x, player_->position().z);
  top_left -= pos;
  bottom_right -= pos;

  glm::vec2 lft = glm::rotate(view_direction, glm::radians(player_->fov()));
  glm::vec2 rgt = glm::rotate(view_direction, -glm::radians(player_->fov()));

  // std::cout << "lft x: "    << lft.x          << " y:" << lft.y          << std::endl;
  // std::cout << "rgt x: "    << rgt.x          << " y:" << rgt.y          << std::endl;
  // std::cout << "toplft x: " << top_left.x     << " y:" << top_left.y     << std::endl;
  // std::cout << "btmrgt x: " << bottom_right.x << " y:" << bottom_right.y << std::endl;

  if (IsInsideFrustum(lft, rgt, top_left)    ) return true;
  if (IsInsideFrustum(lft, rgt, bottom_right)) return true;
  if (IsInsideFrustum(lft, rgt, glm::vec2(top_left.x, bottom_right.y))) return true;
  if (IsInsideFrustum(lft, rgt, glm::vec2(bottom_right.x, top_left.y))) return true;
  return false;
}

glm::vec2 Clipmap::QuadIntersection(
//  glm::vec2 a, glm::vec2 b, glm::vec2 top_lft, glm::vec2 bot_rgt
) {
  // glm::vec2 view_direction = glm::vec2(
  //   cos(glm::radians(player_->horizontal_angle())),
  //   sin(glm::radians(player_->horizontal_angle()))
  // );

  // glm::vec2 lft = player_->position() + glm::rotate(view_direction, glm::radians(player_->fov()));
  // glm::vec2 rgt = player_->position() + glm::rotate(view_direction, -glm::radians(player_->fov()));



  // detail::tvec2<T> glm::rotate    (       detail::tvec2< T > const &      v,
  //   T const &       angle 
  // )       
  //  
  //  
  // ProjectionMatrix = glm::perspective(glm::radians(player_->fov()), 4.0f / 3.0f, 20.0f, 2000000.0f);

  // glm::vec2 frustum_top
  // glm::vec2 frustum_bot

  // LineIntersection(glm::vec2 a[2], glm::vec2 b[2], glm::vec2* intersection) {
  return glm::vec2(0, 0);
}

void Clipmap::Init() {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);
  glGenBuffers(1, &barycentric_buffer_);
  glGenBuffers(5, subregion_buffers_);

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

  for (int i = 0; i < 5; i++) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subregion_buffers_[i]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1) * sizeof(unsigned int), subregion_indices_[i], GL_STATIC_DRAW);
  }

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

  glGenTextures(1, &valid_texture_);
  glBindTexture(GL_TEXTURE_RECTANGLE, valid_texture_);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RED, CLIPMAP_SIZE+1, CLIPMAP_SIZE+1, 0, GL_RED, GL_FLOAT, height_buffer_.valid);


  glGenBuffers(1, &center_region_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, center_region_buffer_);

  bool borders[4] = { true, true, true, true };
  center_region_size_ = CreateRenderRegion(glm::ivec2(0, 0), glm::ivec2(CLIPMAP_SIZE, CLIPMAP_SIZE), borders);
  borders[0] = false; borders[1] = false; borders[2] = false; borders[3] = false;

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
            borders[RR_LEFT] = true; borders[RR_TOP] = true; borders[RR_BOTTOM] = true; borders[RR_RIGHT] = false;
            top_left = glm::ivec2(0, 0);
            size = glm::ivec2(left_size, CLIPMAP_SIZE);
            break;
          case RR_TOP:
            borders[RR_LEFT] = false; borders[RR_TOP] = true; borders[RR_BOTTOM] = false; borders[RR_RIGHT] = false;
            top_left = glm::ivec2(left_size, 0);
            size = glm::ivec2(CLIPMAP_SIZE/2, up_size);
            break;
          case RR_BOTTOM:
            borders[RR_LEFT] = false; borders[RR_TOP] = false; borders[RR_BOTTOM] = true; borders[RR_RIGHT] = false;
            top_left = glm::ivec2(left_size, up_size + CLIPMAP_SIZE/2);
            size = glm::ivec2(CLIPMAP_SIZE/2, CLIPMAP_SIZE/2 - up_size);
            break;
          case RR_RIGHT:
            borders[RR_LEFT] = false; borders[RR_TOP] = true; borders[RR_BOTTOM] = true; borders[RR_RIGHT] = true;
            top_left = glm::ivec2(left_size + CLIPMAP_SIZE / 2, 0);
            size = glm::ivec2(CLIPMAP_SIZE/2 - left_size, CLIPMAP_SIZE);
            break;
          default: throw;
        }
        render_region_top_left_[x][y][region] = top_left;
        render_region_clip_size_[x][y][region] = size;

        render_region_sizes_[x][y][region] = CreateRenderRegion(top_left, size, borders);
      }
    }
  }

  height_map_ = new float[1000000];
  CreateHeightmap();
}

int Clipmap::CreateRenderRegion(glm::ivec2 top_left, glm::ivec2 size, bool borders[4]) {
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

int Clipmap::GetTileSize() {
  return 1 << (level_ - 1);
}

float Clipmap::GetHeight(float x, float y) {
  glm::ivec2 world_coords = glm::ivec2(x, y);

  int point_1 = -1;
  int point_2 = -1;
  int point_3 = -1;
  float distance_1 = 1000000000;
  float distance_2 = 1000000000;
  float distance_3 = 1000000000;
  for (int i = 0; i < 10; i++) {
    float distance = sqrt(pow(feature_points[i].x - world_coords.x, 2) + pow(feature_points[i].y - world_coords.y, 2)); 
    if (distance < distance_1) {
      distance_1 = distance;
      point_1 = i;
    }
  }

  for (int i = 0; i < 10; i++) {
    if (i == point_1) continue;
    float distance = sqrt(pow(feature_points[i].x - world_coords.x, 2) + pow(feature_points[i].y - world_coords.y, 2)); 
    if (distance < distance_2) {
      distance_2 = distance;
      point_2 = i;
    }
  }

  for (int i = 0; i < 10; i++) {
    if (i == point_1) continue;
    if (i == point_2) continue;
    float distance = sqrt(pow(feature_points[i].x - world_coords.x, 2) + pow(feature_points[i].y - world_coords.y, 2)); 
    if (distance < distance_3) {
      distance_3 = distance;
      point_3 = i;
    }
  }

  float h = -1 + (distance_1 - distance_2) / float(10000);

  h = 0.3 * h + 0.7 * (
    2450 * noise_.noise(x * 0.00002, y * 0.00002) +
    1500 * noise_.noise((1000 + x) * 0.00002, (1000 + y) * 0.00002) +
    510 * noise_.noise(x * 0.0001, y * 0.0001) +
    40 * noise_.noise(x * 0.001, y * 0.001) +
    0
  )/ 4000.0f;

  return h;
}

float Clipmap::GetGridPointHeight(int x, int y) {
  glm::ivec2 world_coords = glm::ivec2(x * TILE_SIZE, y * TILE_SIZE);

  int point_1 = -1;
  int point_2 = -1;
  int point_3 = -1;
  float distance_1 = 1000000000;
  float distance_2 = 1000000000;
  float distance_3 = 1000000000;
  for (int i = 0; i < 10; i++) {
    float distance = sqrt(pow(feature_points[i].x - world_coords.x, 2) + pow(feature_points[i].y - world_coords.y, 2)); 
    if (distance < distance_1) {
      distance_1 = distance;
      point_1 = i;
    }
  }

  for (int i = 0; i < 10; i++) {
    if (i == point_1) continue;
    float distance = sqrt(pow(feature_points[i].x - world_coords.x, 2) + pow(feature_points[i].y - world_coords.y, 2)); 
    if (distance < distance_2) {
      distance_2 = distance;
      point_2 = i;
    }
  }

  for (int i = 0; i < 10; i++) {
    if (i == point_1) continue;
    if (i == point_2) continue;
    float distance = sqrt(pow(feature_points[i].x - world_coords.x, 2) + pow(feature_points[i].y - world_coords.y, 2)); 
    if (distance < distance_3) {
      distance_3 = distance;
      point_3 = i;
    }
  }

  return -1 + (distance_1 - distance_2 - distance_3) / float(10000);
  


  // if (x >= -128 && x < 130 && y >= -128 && y < 130) {
  //   x += 128;
  //   y += 128;
  //   std::cout << height_map_[x * 258 + y] << std::endl;
  //   return height_map_[x * 258 + y];
  // }
  // return -1;

  // return GetHeight(x * TILE_SIZE, y * TILE_SIZE);
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

void Clipmap::UpdateHeightMap() {
  if (num_invalid_ <= 0) return;

  for (int y = 0; y < CLIPMAP_SIZE + 1; y++) {
    if (num_invalid_ <= 0) break;
    for (int x = 0; x < CLIPMAP_SIZE + 1; x++) {
      if (num_invalid_ <= 0) break;
      if (height_buffer_.valid[y * (CLIPMAP_SIZE+1) + x]) continue;

      glm::ivec2 grid_coords = BufferToGridCoordinates(glm::ivec2(x, y));
      glm::vec3 world_coords = GridToWorldCoordinates(grid_coords);

      // height_buffer_.height[y * (CLIPMAP_SIZE+1) + x] = float(1 + GetGridPointHeight(grid_coords.x, grid_coords.y)) / 2;
      height_buffer_.height[y * (CLIPMAP_SIZE+1) + x] = float(1 + GetHeight(world_coords.x, world_coords.z)) / 2;
      height_buffer_.valid[y * (CLIPMAP_SIZE+1) + x] = 1;

      float step = GetTileSize() * TILE_SIZE;
      // glm::vec3 a = glm::vec3(0,    8000 * (float(1 + GetGridPointHeight(grid_coords.x       , grid_coords.y        )) / 2), 0);
      // glm::vec3 b = glm::vec3(step, 8000 * (float(1 + GetGridPointHeight(grid_coords.x + 1, grid_coords.y        )) / 2), 0);
      // glm::vec3 c = glm::vec3(0,    8000 * (float(1 + GetGridPointHeight(grid_coords.x       , grid_coords.y + 1)) / 2), step);
      glm::vec3 a = glm::vec3(0,    8000 * (float(1 + GetHeight(world_coords.x       , world_coords.z        )) / 2), 0);
      glm::vec3 b = glm::vec3(step, 8000 * (float(1 + GetHeight(world_coords.x + step, world_coords.z        )) / 2), 0);
      glm::vec3 c = glm::vec3(0,    8000 * (float(1 + GetHeight(world_coords.x       , world_coords.z + step )) / 2), step);
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

void Clipmap::Update(glm::vec3 player_pos) {
  glm::ivec2 grid_coords = WorldToGridCoordinates(player_pos);
  glm::ivec2 new_top_left = ClampGridCoordinates(grid_coords, GetTileSize()) - CLIPMAP_OFFSET * GetTileSize();

  InvalidateOuterBuffer(new_top_left);
  if (top_left_ == new_top_left && num_invalid_ == 0) return;
  top_left_ = new_top_left;

  UpdateHeightMap();

  // glBindTexture(GL_TEXTURE_RECTANGLE, valid_texture_);
  // glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, (CLIPMAP_SIZE + 1), (CLIPMAP_SIZE + 1), GL_RED, GL_FLOAT, height_buffer_.valid);
}

void Clipmap::Render(
  glm::vec3 player_pos, 
  Shader* shader, 
  glm::mat4 ProjectionMatrix, 
  glm::mat4 ViewMatrix,
  bool full
) {
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
  shader->BindBuffer(barycentric_buffer_, 2, 4);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_RECTANGLE, normals_texture_[active_texture_]);
  glUniform1i(shader->GetUniformId("NormalsSampler"), 5);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_RECTANGLE, height_texture_[active_texture_]);
  glUniform1i(shader->GetUniformId("HeightMapSampler"), 4);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_RECTANGLE, valid_texture_);
  glUniform1i(shader->GetUniformId("ValidSampler"), 3);

  if (full) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, center_region_buffer_);
    // glDrawElements(GL_TRIANGLE_STRIP, center_region_size_, GL_UNSIGNED_INT, (void*) 0);
    glDrawElements(GL_TRIANGLES, center_region_size_, GL_UNSIGNED_INT, (void*) 0);

    // When i need to offset look here.
    // glDrawElements(GL_TRIANGLE_STRIP, 54, GL_UNSIGNED_INT, (void*) (44 * sizeof(unsigned int)));
  } else {
    glm::ivec2 grid_coords = WorldToGridCoordinates(player_pos);
    glm::ivec2 clipmap_offset = ClampGridCoordinates(grid_coords, GetTileSize() >> 1);
    clipmap_offset -= ClampGridCoordinates(grid_coords, GetTileSize());
    clipmap_offset /= GetTileSize();

    for (int region = 0 ; region < 4; region++) {
      glm::vec2 top_lft = top_left_ * TILE_SIZE + render_region_top_left_[clipmap_offset.x][clipmap_offset.y][region] * GetTileSize() * TILE_SIZE;
      glm::vec2 bot_rgt = top_lft + glm::vec2(render_region_clip_size_[clipmap_offset.x][clipmap_offset.y][region] * GetTileSize() * TILE_SIZE);
      // if (IsSubregionVisible(top_lft, bot_rgt)) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_region_buffers_[clipmap_offset.x][clipmap_offset.y][region]);
        // glDrawElements(GL_TRIANGLE_STRIP, render_region_sizes_[clipmap_offset.x][clipmap_offset.y][region], GL_UNSIGNED_INT, (void*) 0);
        glDrawElements(GL_TRIANGLES, render_region_sizes_[clipmap_offset.x][clipmap_offset.y][region], GL_UNSIGNED_INT, (void*) 0);
      // }
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
    clipmaps_[i] = Clipmap(player_, i + 1); 

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
  update_counter_ = 0;
  // for (int i = CLIPMAP_LEVELS - 1; i >= 0; i--) {
  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i].Update(player_->position());
  }

  bool first = true;
  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    // if (clipmaps_[i].num_invalid() == 0) {
    clipmaps_[i].Render(player_->position(), &shader_, ProjectionMatrix, ViewMatrix, first);
    first = false;
    // }
  }

  shader_.Clear();
}

} // End of namespace.
