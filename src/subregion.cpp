#include "subregion.hpp"

namespace Sibyl {

const short Subregion::BORDERS[] = { 14, 4, 2, 7, 15 };

Subregion::Subregion(
  SubregionLabel subregion,
  int clipmap_level
  ) : subregion_(subregion), clipmap_level_(clipmap_level) {
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

  for (int y = start.y; y < end.y; y++) {
    for (int x = start.x; x < end.x; x++) {
      CreateTile(indices, x, y);
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

void Subregion::Draw(glm::ivec2 offset, glm::ivec2 clipmap_top_lft, bool water) {
  int num_tiles = 1 << (clipmap_level_ - 1);
  glm::ivec2 top_lft = clipmap_top_lft * TILE_SIZE + top_left_[offset.x][offset.y] * num_tiles * TILE_SIZE;
  glm::ivec2 bot_rgt = top_lft + size_[offset.x][offset.y] * num_tiles * TILE_SIZE;
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_[offset.x][offset.y]);
  if (subregion_ == SUBREGION_CENTER) {
    glDrawElements(GL_TRIANGLES, buffer_size_[offset.x][offset.y], GL_UNSIGNED_INT, (void*) 0);
  } else {
    int buffer_size  = size_[offset.x][offset.y].x * size_[offset.x][offset.y].y * 6;
    glDrawElements(GL_TRIANGLES, buffer_size, GL_UNSIGNED_INT, (void*) (0 * sizeof(unsigned int)));
  }
}

} // End of namespace.
