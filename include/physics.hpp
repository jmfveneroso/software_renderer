#ifndef __PHYSICS_HPP__
#define __PHYSICS_HPP__

#include "entity_manager.hpp"
#include "player.hpp"

namespace Sibyl {

struct AABB {
  glm::vec3 min, max;
};

class Physics {
  std::shared_ptr<EntityManager> entity_manager_;
  std::shared_ptr<Player> player_;

  bool TestCollisionAABB(glm::vec3 last_pos, AABB aabb, glm::vec3 triangle_points[]);
  void TestCollision(glm::vec3 last_pos, std::shared_ptr<IEntity> entity);
  void TestCollisionTerrain();

 public:
  Physics(
    std::shared_ptr<EntityManager>,
    std::shared_ptr<Player>
  );

  void UpdateForces();
};

} // End of namespace.

#endif
