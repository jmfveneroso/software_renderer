#include "subregion.hpp"

namespace Sibyl {

const short Subregion::BORDERS[] = { 14, 4, 2, 7, 15 };

Subregion::Subregion(
  std::shared_ptr<Player> player, 
  SubregionLabel subregion,
  int clipmap_level
  ) : player_(player), subregion_(subregion), clipmap_level_(clipmap_level) {
  Init();
}

void Subregion::Init() {
  for (int x = 0; x < 2; x++) {
    for (int y = 0; y < 2; y++) {
      int left_size = CLIPMAP_SIZE / 4 + x;
      int up_size   = CLIPMAP_SIZE / 4 + y;
      switch (subregion_) {
        case SUBREGION_LEFT:
          top_left_[x][y] = glm::ivec2(0, 0);
          size_[x][y] = glm::ivec2(left_size, CLIPMAP_SIZE);
          break;
        case SUBREGION_TOP:
          top_left_[x][y] = glm::ivec2(left_size, 0);
          size_[x][y] = glm::ivec2(CLIPMAP_SIZE / 2, up_size);
          break;
        case SUBREGION_BOTTOM:
          top_left_[x][y] = glm::ivec2(left_size, up_size + CLIPMAP_SIZE / 2);
          size_[x][y] = glm::ivec2(CLIPMAP_SIZE / 2, CLIPMAP_SIZE / 2 - up_size);
          break;
        case SUBREGION_RIGHT:
          top_left_[x][y] = glm::ivec2(left_size + CLIPMAP_SIZE / 2, 0);
          size_[x][y] = glm::ivec2(CLIPMAP_SIZE / 2 - left_size, CLIPMAP_SIZE);
          break;
        case SUBREGION_CENTER:
          top_left_[x][y] = glm::ivec2(left_size, up_size);
          size_[x][y] = glm::ivec2(CLIPMAP_SIZE / 2, CLIPMAP_SIZE / 2);
          break;
        default: throw;
      }

      CreateBuffer(glm::ivec2(x, y));
    }
  }
}

void Subregion::CreateTopBorder(std::vector<unsigned int>& indices, int x, int y) {
  if (x % 2 == 0) {
    if (x == 0 && (BORDERS[subregion_] & LEFT_BORDER)) {
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
    if (x == CLIPMAP_SIZE - 1 && (BORDERS[subregion_] & RIGHT_BORDER)) {
      indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
      indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
      indices.push_back((y + 2) * (CLIPMAP_SIZE + 1) + (x + 1));
    } else {
      indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
      indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
      indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
    }
  }
}

void Subregion::CreateBottomBorder(std::vector<unsigned int>& indices, int x, int y) {
  if (x % 2 == 0) {
    if (!(BORDERS[subregion_] & LEFT_BORDER) || x != 0) {
      indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
      indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
      indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
    }
    indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
    indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
    indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 2));
  } else {
    if ((BORDERS[subregion_] & RIGHT_BORDER) && x == CLIPMAP_SIZE - 1) return;
    indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
    indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
    indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
  }
}

void Subregion::CreateLeftBorder(std::vector<unsigned int>& indices, int x, int y) {
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
}

void Subregion::CreateRightBorder(std::vector<unsigned int>& indices, int x, int y) {
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
}

void Subregion::CreateTile(std::vector<unsigned int>& indices, int x, int y) {
  if ((BORDERS[subregion_] & TOP_BORDER) && y == 0) {
    return CreateTopBorder(indices, x, y);
  }

  if ((BORDERS[subregion_] & BOTTOM_BORDER) && y == CLIPMAP_SIZE - 1) {
    return CreateBottomBorder(indices, x, y);
  }

  if ((BORDERS[subregion_] & LEFT_BORDER) && x == 0) {
    return CreateLeftBorder(indices, x, y);
  }

  if ((BORDERS[subregion_] & RIGHT_BORDER) && x == CLIPMAP_SIZE - 1) {
    return CreateRightBorder(indices, x, y);
  }

  // Checkerboard pattern.
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

void Subregion::CreateBuffer(glm::ivec2 offset) {
  std::vector<unsigned int> indices;
  glm::ivec2 start = top_left_[offset.x][offset.y];
  glm::ivec2 end   = start + size_[offset.x][offset.y];

  if (subregion_ == SUBREGION_LEFT || subregion_ == SUBREGION_RIGHT) {
    for (int y = start.y; y < end.y; y++) {
      for (int x = start.x; x < end.x; x++) {
        CreateTile(indices, x, y);
      } 
    }
  } else {
    for (int x = start.x; x < end.x; x++) {
      for (int y = start.y; y < end.y; y++) {
        CreateTile(indices, x, y);
      } 
    }
  }

  glGenBuffers(1, &buffer_[offset.x][offset.y]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_[offset.x][offset.y]);
  buffer_size_[offset.x][offset.y] = indices.size();

  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices.size() * sizeof(unsigned int), 
    &indices[0], 
    GL_STATIC_DRAW
  );
}

bool Subregion::IsInsideFrustum(glm::vec2 lft, glm::vec2 rgt, glm::vec2 p) {
  double denominator = glm::determinant(glm::mat2(lft, rgt));
  double alpha = glm::determinant(glm::mat2(p, rgt)) / denominator;
  double beta  = glm::determinant(glm::mat2(lft, p)) / denominator;
  return alpha >= 0 && beta >= 0;
}

bool Subregion::IsSubregionVisible(glm::ivec2 offset, glm::vec2 top_lft, glm::vec2 bot_rgt) {
  float h = max_height_[offset.x][offset.y];
  glm::vec3 top_plane[4];
  top_plane[0] = glm::vec3(top_lft.x, h, top_lft.y);
  top_plane[1] = glm::vec3(bot_rgt.x, h, top_lft.y);
  top_plane[2] = glm::vec3(top_lft.x, h, bot_rgt.y);
  top_plane[3] = glm::vec3(bot_rgt.x, h, bot_rgt.y);

  glm::vec3 bot_plane[4];
  h = min_height_[offset.x][offset.y];
  top_plane[0] = glm::vec3(top_lft.x, h, top_lft.y);
  top_plane[1] = glm::vec3(bot_rgt.x, h, top_lft.y);
  top_plane[2] = glm::vec3(top_lft.x, h, bot_rgt.y);
  top_plane[3] = glm::vec3(bot_rgt.x, h, bot_rgt.y);

  // glm::vec3 plane_normal = player_->GetFrustumPlane(FRUSTUM_PLANE_RIGHT);


  glm::vec2 view_direction = glm::vec2(
    sin(player_->horizontal_angle()),
    cos(player_->horizontal_angle())
  );

  glm::vec2 pos = glm::vec2(player_->position().x, player_->position().z);
  top_lft -= pos;
  bot_rgt -= pos;

  glm::vec2 lft = glm::rotate(view_direction, glm::radians(player_->fov()));
  glm::vec2 rgt = glm::rotate(view_direction, -glm::radians(player_->fov()));

  if (IsInsideFrustum(lft, rgt, top_lft)    ) return true;
  if (IsInsideFrustum(lft, rgt, bot_rgt)) return true;
  if (IsInsideFrustum(lft, rgt, glm::vec2(top_lft.x, bot_rgt.y))) return true;
  if (IsInsideFrustum(lft, rgt, glm::vec2(bot_rgt.x, top_lft.y))) return true;
  return false;
}

void Subregion::Draw(glm::ivec2 offset, glm::vec2 clipmap_top_lft) {
  int num_tiles = 1 << (clipmap_level_ - 1);
  glm::vec2 top_lft = clipmap_top_lft * float(TILE_SIZE) + glm::vec2(top_left_[offset.x][offset.y]) * float(num_tiles * TILE_SIZE);
  glm::vec2 bot_rgt = top_lft + glm::vec2(size_[offset.x][offset.y]) * float(num_tiles * TILE_SIZE);
  if (!IsSubregionVisible(offset, top_lft, bot_rgt)) return;

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_[offset.x][offset.y]);
  glDrawElements(GL_TRIANGLES, buffer_size_[offset.x][offset.y], GL_UNSIGNED_INT, (void*) 0);
}

void Subregion::UpdateHeight(int offset_x, int offset_y, float height) {
  for (int y = 0; y < 2; y++) {
    for (int x = 0; x < 2; x++) {
      glm::ivec2 start = top_left_[x][y];
      glm::ivec2 end   = start + size_[x][y];
      if (x < start.x || x >= end.x || y < start.y || y >= end.y) continue;
      if (height > max_height_[x][y]) max_height_[x][y] = height;
      if (height < min_height_[x][y]) min_height_[x][y] = height;
    }
  }
}

void Subregion::Clear() {
  for (int offset_y = 0; offset_y < 2; offset_y++) {
    for (int offset_x = 0; offset_x < 2; offset_x++) {
      max_height_[offset_x][offset_y] = -999999.0f; 
      min_height_[offset_x][offset_y] = 999999.0f; 
    }
  }
}

} // End of namespace.
