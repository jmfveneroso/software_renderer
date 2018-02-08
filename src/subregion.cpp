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
    indices.push_back(0);
    indices.push_back(0);
    indices.push_back(0);
  }
}

void Subregion::CreateBottomBorder(std::vector<unsigned int>& indices, int x, int y) {
  if (x % 2 == 0) {
    if (!(BORDERS[subregion_] & LEFT_BORDER) || x != 0) {
      indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
      indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
      indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
    } else {
      indices.push_back(0);
      indices.push_back(0);
      indices.push_back(0);
    }
    indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
    indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + x);
    indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 2));
  } else {
    if ((BORDERS[subregion_] & RIGHT_BORDER) && x == CLIPMAP_SIZE - 1) {
      indices.push_back(0);
      indices.push_back(0);
      indices.push_back(0);
    } else {
      indices.push_back(y * (CLIPMAP_SIZE + 1) + x);
      indices.push_back((y + 1) * (CLIPMAP_SIZE + 1) + (x + 1));
      indices.push_back(y * (CLIPMAP_SIZE + 1) + (x + 1));
    }
    indices.push_back(0);
    indices.push_back(0);
    indices.push_back(0);
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
    indices.push_back(0);
    indices.push_back(0);
    indices.push_back(0);
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
    indices.push_back(0);
    indices.push_back(0);
    indices.push_back(0);
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

std::vector<glm::vec3> Subregion::GetEnclosingCubePlane(
  glm::ivec2 top_lft, glm::ivec2 bot_rgt, 
  float min_height, float max_height,
  int plane_num
) {
  std::vector<glm::vec3> points;

  switch (plane_num) {
    case 0: // Top.
      points.push_back(glm::vec3(top_lft.x, max_height, top_lft.y));
      points.push_back(glm::vec3(top_lft.x, max_height, bot_rgt.y));
      points.push_back(glm::vec3(bot_rgt.x, max_height, bot_rgt.y));
      points.push_back(glm::vec3(bot_rgt.x, max_height, top_lft.y));
      break;
    case 1: // Left.
      points.push_back(glm::vec3(top_lft.x, max_height, top_lft.y));
      points.push_back(glm::vec3(top_lft.x, min_height, top_lft.y));
      points.push_back(glm::vec3(top_lft.x, min_height, bot_rgt.y));
      points.push_back(glm::vec3(top_lft.x, max_height, bot_rgt.y));
      break;
    case 2: // Front.
      points.push_back(glm::vec3(top_lft.x, max_height, bot_rgt.y));
      points.push_back(glm::vec3(top_lft.x, min_height, bot_rgt.y));
      points.push_back(glm::vec3(bot_rgt.x, min_height, bot_rgt.y));
      points.push_back(glm::vec3(bot_rgt.x, max_height, bot_rgt.y));
      break;
    case 3: // Right.
      points.push_back(glm::vec3(bot_rgt.x, max_height, bot_rgt.y));
      points.push_back(glm::vec3(bot_rgt.x, min_height, bot_rgt.y));
      points.push_back(glm::vec3(bot_rgt.x, min_height, top_lft.y));
      points.push_back(glm::vec3(bot_rgt.x, max_height, top_lft.y));
      break;
    case 4: // Back.
      points.push_back(glm::vec3(bot_rgt.x, max_height, top_lft.y));
      points.push_back(glm::vec3(bot_rgt.x, min_height, top_lft.y));
      points.push_back(glm::vec3(top_lft.x, min_height, top_lft.y));
      points.push_back(glm::vec3(top_lft.x, max_height, top_lft.y));
      break;
    case 5: // Bottom.
      points.push_back(glm::vec3(top_lft.x, min_height, top_lft.y));
      points.push_back(glm::vec3(top_lft.x, min_height, bot_rgt.y));
      points.push_back(glm::vec3(bot_rgt.x, min_height, bot_rgt.y));
      points.push_back(glm::vec3(bot_rgt.x, min_height, top_lft.y));
      break;
    default: throw;
  }

  return points;
}

std::vector<glm::vec3> Subregion::GetClippedEnclosingCube(
  glm::ivec2 top_lft, glm::ivec2 bot_rgt, 
  float min_height, float max_height
) {
  std::vector<glm::vec3> clipped_cube;

  // For each plane.
  for (int i = 0; i < 6; i++) {
    std::vector<glm::vec3> cube_plane = GetEnclosingCubePlane(top_lft, bot_rgt, min_height, max_height, i);

    // Clip against all frustum planes.
    for (int j = 0; j < 4; j++) {
      Plane clipping_plane = player_->GetFrustumPlane(static_cast<FrustumPlane>(j));
      cube_plane = Geometry::ClipPlane(cube_plane, clipping_plane);
    }
    clipped_cube.insert(clipped_cube.end(), cube_plane.begin(), cube_plane.end());
  }

  return clipped_cube;
}

void Subregion::DrawElements(glm::ivec2 offset, glm::ivec2 top_lft, glm::ivec2 bot_rgt, glm::ivec2 clipmap_top_left) {
  float min_height = (min_height_[offset.x][offset.y] * 2 - 1) * MAX_HEIGHT;
  float max_height = (max_height_[offset.x][offset.y] * 2 - 1) * MAX_HEIGHT;

  std::vector<glm::vec3> clipped_cube = GetClippedEnclosingCube(top_lft, bot_rgt, min_height, max_height);
  if (clipped_cube.size() == 0) {
    return;
  }
 
  // The left and right regions have horizonal quad strips. Whie the top and bottom
  // regions have vertical quad strips.
  bool vertical_strips = subregion_ == SUBREGION_TOP || subregion_ == SUBREGION_BOTTOM;

  // If the quad strips are vertical, we need to find the min and max x coordinates.
  // If the quad strips are horizontal, we need to find the min and max z coordinates.
  int min_x = 1000000000;
  int max_x = -1000000000;
  int min_z = 1000000000;
  int max_z = -1000000000;
  for (auto& p : clipped_cube) {
    if      (p.x < min_x) min_x = p.x;
    else if (p.x > max_x) max_x = p.x;
    if      (p.z < min_z) min_z = p.z;
    else if (p.z > max_z) max_z = p.z;
  }

  int num_tiles = 1 << (clipmap_level_ - 1);
  int buffer_start;
  int buffer_size;
  if (vertical_strips) {
    int min = (min_x - top_lft.x) / (TILE_SIZE * num_tiles);
    int max = (max_x - top_lft.x) / (TILE_SIZE * num_tiles);
    if (min < 0) throw; 
    if (max > size_[offset.x][offset.y].x) throw;
    min = (min > 0) ? min : 0;
    max = (max < size_[offset.x][offset.y].x) ? max : size_[offset.x][offset.y].x;
    buffer_start = min * size_[offset.x][offset.y].y * 6;
    buffer_size  = (max - min) * size_[offset.x][offset.y].y * 6;

  } else {
    int min = (min_z - top_lft.y) / (TILE_SIZE * num_tiles);
    int max = (max_z - top_lft.y) / (TILE_SIZE * num_tiles);
    if (min < 0) throw; 
    if (max > size_[offset.x][offset.y].y) throw;
    min = (min > 0) ? min : 0;
    max = (max < size_[offset.x][offset.y].y) ? max : size_[offset.x][offset.y].y;
    buffer_start = min * size_[offset.x][offset.y].x * 6;
    buffer_size  = (max - min) * size_[offset.x][offset.y].x * 6;
  }

  glDrawElements(GL_TRIANGLES, buffer_size, GL_UNSIGNED_INT, (void*) (buffer_start * sizeof(unsigned int)));
}

void Subregion::Draw(glm::ivec2 offset, glm::ivec2 clipmap_top_lft, bool water) {
  float h = (min_height_[offset.x][offset.y] * 2 - 1) * MAX_HEIGHT;
  if (water && h > 100.0f) return;

  int num_tiles = 1 << (clipmap_level_ - 1);
  glm::ivec2 top_lft = clipmap_top_lft * TILE_SIZE + top_left_[offset.x][offset.y] * num_tiles * TILE_SIZE;
  glm::ivec2 bot_rgt = top_lft + size_[offset.x][offset.y] * num_tiles * TILE_SIZE;
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_[offset.x][offset.y]);
  if (subregion_ == SUBREGION_CENTER) {
    glDrawElements(GL_TRIANGLES, buffer_size_[offset.x][offset.y], GL_UNSIGNED_INT, (void*) 0);
  } else {
    DrawElements(offset, top_lft, bot_rgt, clipmap_top_lft);
  }
}

void Subregion::UpdateHeight(glm::ivec2 clipmap_top_lft, int offset_x, int offset_y, float height) {
  for (int y = 0; y < 2; y++) {
    for (int x = 0; x < 2; x++) {
      max_height_[x][y] = 1.0;
      min_height_[x][y] = -1.0;
    }
  }
}

void Subregion::Clear() {
  for (int offset_y = 0; offset_y < 2; offset_y++) {
    for (int offset_x = 0; offset_x < 2; offset_x++) {
      max_height_[offset_x][offset_y] = -2.0f; 
      min_height_[offset_x][offset_y] = 2.0f; 
    }
  }
}

} // End of namespace.
