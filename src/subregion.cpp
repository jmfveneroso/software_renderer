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

Segment Subregion::ClipSegToFrustum(Segment seg) {
  for (int i = 0; i < 4; i++) {
    Plane plane = player_->GetFrustumPlane(static_cast<FrustumPlane>(i));
    switch (Geometry::GetPosRelativeToPlane(plane, seg)) {
      case PLANE_FRONT:
        // Entirely in front of one of the frustum planes.
        break;
      case PLANE_BACK:
        // Entirely behind one of the frustum planes.
        if (i == FRUSTUM_PLANE_UP || i == FRUSTUM_PLANE_DOWN) break;
        seg = Segment(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)); 
        seg.null = true;
        return seg;
      case PLANE_INTERSECT:
        // Intersects one of the frustum planes.
        if (Geometry::GetPosRelativeToPlane(plane, seg.p1) == PLANE_FRONT) {
          glm::vec3 p = Geometry::GetSegPlaneIntersection(seg, plane);
          seg = Segment(p, seg.p2);
        } else {
          glm::vec3 p = Geometry::GetSegPlaneIntersection(seg, plane);
          seg = Segment(seg.p1, p);
        }
        break;
    }
  }
  return seg;
}

Segment Subregion::GetEnclosingSegment(Segment seg1, Segment seg2, bool vertical) {
  if (seg1.null) {
    return seg2;
  } else if (seg2.null) {
    return seg1;
  }

  Segment seg;
  seg.null = false;
  if (vertical) {
    seg.p1 = (seg1.p1.z < seg2.p1.z) ? seg1.p1 : seg2.p1;
    seg.p2 = (seg1.p2.z > seg2.p2.z) ? seg1.p2 : seg2.p2;
  } else {
    seg.p1 = (seg1.p1.x < seg2.p1.x) ? seg1.p1 : seg2.p1;
    seg.p2 = (seg1.p2.x > seg2.p2.x) ? seg1.p2 : seg2.p2;
  }
  return seg;
}

Segment Subregion::ClipSubregion(glm::ivec2 offset, glm::vec2 top_lft, glm::vec2 bot_rgt, float height) {
  Segment segs[2];

  bool vertical = subregion_ == SUBREGION_LEFT || subregion_ == SUBREGION_RIGHT;
  if (vertical) {
    segs[0] = Segment(glm::vec3(top_lft.x, height, top_lft.y), glm::vec3(bot_rgt.x, height, top_lft.y));
    segs[1] = Segment(glm::vec3(top_lft.x, height, bot_rgt.y), glm::vec3(bot_rgt.x, height, bot_rgt.y));
  // Vertical triangle strips.
  } else {
    segs[0] = Segment(glm::vec3(top_lft.x, height, top_lft.y), glm::vec3(top_lft.x, height, bot_rgt.y));
    segs[1] = Segment(glm::vec3(bot_rgt.x, height, top_lft.y), glm::vec3(bot_rgt.x, height, bot_rgt.y));
  }
 
  if (segs[0].null && segs[1].null) return Segment();

  segs[0] = ClipSegToFrustum(segs[0]);
  segs[1] = ClipSegToFrustum(segs[1]);

  return GetEnclosingSegment(segs[0], segs[1], vertical);
}

void Subregion::DrawElements(glm::ivec2 offset, glm::vec2 top_lft, glm::vec2 bot_rgt, glm::vec2 clipmap_top_left) {
  // player_->set_vertical_angle(0.0f);
  // player_->set_horizontal_angle(0.0f);
  // if (subregion_ == SUBREGION_LEFT && clipmap_level_ == 2) {
    float min_height = (min_height_[offset.x][offset.y] * 2 - 1) * MAX_HEIGHT;
    float max_height = (max_height_[offset.x][offset.y] * 2 - 1) * MAX_HEIGHT;
    min_height = 0;
    max_height = 0;
    bool vertical = subregion_ == SUBREGION_LEFT || subregion_ == SUBREGION_RIGHT;
    Segment seg1 = ClipSubregion(offset, top_lft, bot_rgt, min_height);
    Segment seg2 = ClipSubregion(offset, top_lft, bot_rgt, max_height);
    Segment seg = GetEnclosingSegment(seg1, seg2, vertical);
    // std::cout << "seg1.p1.x: " << seg1.p1.x << " seg1.p1.y: " << seg1.p1.y << " seg1.p2.x: " << seg1.p2.x << " seg1.p2.y: " << seg1.p2.y << std::endl;
    // std::cout << "seg2.p1.x: " << seg2.p1.x << " seg2.p1.y: " << seg2.p1.y << " seg2.p2.x: " << seg2.p2.x << " seg2.p2.y: " << seg2.p2.y << std::endl;

    // Plane plane = player_->GetFrustumPlane(FRUSTUM_PLANE_UP);
    // std::cout << "FRUSTUM_PLANE_UP" << std::endl;
    // std::cout << "plane.vec1.x: " << plane.vec1.x << " plane.vec1.y: " << plane.vec1.y << " plane.vec1.z: " << plane.vec1.z << std::endl;
    // std::cout << "plane.vec2.x: " << plane.vec2.x << " plane.vec2.y: " << plane.vec2.y << " plane.vec2.z: " << plane.vec2.z << std::endl;

    // std::cout << "clipmap_level: " << clipmap_level_ << std::endl;
    // switch (subregion_) {
    //   case SUBREGION_LEFT: 
    //     std::cout << "subregion_label: SUBREGION_LEFT" << std::endl;
    //     break;
    //   case SUBREGION_TOP:
    //     std::cout << "subregion_label: SUBREGION_TOP" << std::endl;
    //     break;
    //   case SUBREGION_BOTTOM:
    //     std::cout << "subregion_label: SUBREGION_BOTTOM" << std::endl;
    //     break;
    //   case SUBREGION_RIGHT:
    //     std::cout << "subregion_label: SUBREGION_RIGHT" << std::endl;
    //     break;
    //   case SUBREGION_CENTER:
    //     std::cout << "subregion_label: SUBREGION_CENTER" << std::endl;
    //     break;
    // }
    // std::cout << "top_lft.x: " << top_lft.x << " top_lft.y: " << top_lft.y << std::endl;
    // std::cout << "bot_rgt.x: " << bot_rgt.x << " bot_rgt.y: " << bot_rgt.y << std::endl;
    // std::cout << "=======================================" << std::endl;
  // }

  // if (seg.null) return;
  glDrawElements(GL_TRIANGLES, buffer_size_[offset.x][offset.y], GL_UNSIGNED_INT, (void*) 0);
  return;

  // // When i need to offset look here.
  // int num_tiles = 1 << (clipmap_level_ - 1);
  // int start, end;
  // if (vertical) {
  //   start = seg.p1.z / float(num_tiles * TILE_SIZE) - clipmap_top_left.y;
  //   end = seg.p2.z / float(num_tiles * TILE_SIZE) - clipmap_top_left.y + 1;
  // } else {
  //   start = seg.p1.x / float(num_tiles * TILE_SIZE) - clipmap_top_left.x;
  //   end = seg.p2.x / float(num_tiles * TILE_SIZE) - clipmap_top_left.x + 1;
  // }

  // start = start * (CLIPMAP_SIZE / 4); 
  // end = end * (CLIPMAP_SIZE / 4); 

  // glDrawElements(GL_TRIANGLE_STRIP, end - start, GL_UNSIGNED_INT, (void*) (start * sizeof(unsigned int)));
}

void Subregion::Draw(glm::ivec2 offset, glm::vec2 clipmap_top_lft) {
  int num_tiles = 1 << (clipmap_level_ - 1);
  glm::vec2 top_lft = clipmap_top_lft * float(TILE_SIZE) + glm::vec2(top_left_[offset.x][offset.y]) * float(num_tiles * TILE_SIZE);
  glm::vec2 bot_rgt = top_lft + glm::vec2(size_[offset.x][offset.y]) * float(num_tiles * TILE_SIZE);
  
  // if (!IsSubregionVisible(offset, top_lft, bot_rgt)) return;

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_[offset.x][offset.y]);
  DrawElements(offset, top_lft, bot_rgt, clipmap_top_lft);
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
