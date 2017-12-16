#ifndef __PHYSICS_HPP__
#define __PHYSICS_HPP__

#include "entity_manager.hpp"

namespace Sibyl {

class Physics {
  std::shared_ptr<EntityManager> entity_manager_;

 public:
  Physics(std::shared_ptr<EntityManager>);

  bool TestCollisionAABB(glm::vec3* player_pos, glm::vec3 last_pos, AABB aabb, glm::vec3 triangle_points[]);
  void TestCollision(glm::vec3* player_pos, glm::vec3 last_pos, std::shared_ptr<IEntity> entity);
  void Collide(glm::vec3*, glm::vec3);
};

} // End of namespace.

#endif
