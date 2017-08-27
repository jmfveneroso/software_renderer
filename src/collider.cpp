#ifndef COLLIDER_H
#define COLLIDER_H

#include <glm/gtx/norm.hpp>

class AABB {
 public:
  glm::vec3 min, max;

  AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max) {
  }

  bool Collides(AABB aabb) {
    if (max.x < aabb.min.x) return false;
  }
};

#endif
