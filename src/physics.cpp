#include "physics.hpp"

namespace Sibyl {

Physics::Physics(
  std::shared_ptr<EntityManager> entity_manager,
  std::shared_ptr<Player> player
) : entity_manager_(entity_manager),
    player_(player) {
}

bool Physics::TestCollisionAABB(glm::vec3 last_pos, AABB aabb, glm::vec3 triangle_points[]) {
  glm::vec3 player_pos = player_->position();

  int min_x = player_pos.x - 0.5f;
  int max_x = player_pos.x + 0.5f;
  int min_y = player_pos.y - 20.0f;
  int max_y = player_pos.y + 1.0f;
  int min_z = player_pos.z - 0.5f;
  int max_z = player_pos.z + 0.5f;

  int cube_min_x = aabb.min.x;
  int cube_max_x = aabb.max.x;
  int cube_min_y = aabb.min.y;
  int cube_max_y = aabb.max.y;
  int cube_min_z = aabb.min.z;
  int cube_max_z = aabb.max.z;

  if (max_x < cube_min_x) return false;
  if (min_x > cube_max_x) return false;
  if (max_y < cube_min_y) return false;
  if (min_y > cube_max_y) return false;
  if (max_z < cube_min_z) return false;
  if (min_z > cube_max_z) return false;

  AABB player_aabb;
  player_aabb.min= glm::vec3(player_pos.x - 0.5f, player_pos.y - 20.0f, player_pos.z - 0.5f);
  player_aabb.max = glm::vec3(player_pos.x + 0.5f, player_pos.y + 1.0f, player_pos.z + 0.5f);

  glm::vec3 player_aabb_center = (player_aabb.min + player_aabb.max) * 0.5f;
  glm::vec3 e = player_aabb.max - player_aabb_center;
  glm::vec3 triangle_normal = glm::normalize(glm::cross(triangle_points[1] - triangle_points[0], triangle_points[2] - triangle_points[1]));
  float d = triangle_normal.x * triangle_points[0].x + triangle_normal.y * triangle_points[0].y + triangle_normal.z * triangle_points[0].z;

  float r = e.x * fabs(triangle_normal.x) + e.y * fabs(triangle_normal.y) + e.z * fabs(triangle_normal.z);
  float s = glm::dot(triangle_normal, player_aabb_center) - d;

  if (fabs(s) <= r) {
    glm::vec3 new_pos = player_pos + triangle_normal * (r - s);
    if (glm::distance(new_pos, last_pos) > glm::distance(player_pos, last_pos)) return false;

    if (triangle_normal.y > 0.65f) player_->set_over_ground(true);
 
    player_->set_position(new_pos);
    return true;
  }
  return false;
}

void Physics::TestCollision(glm::vec3 last_pos, std::shared_ptr<IEntity> entity) {
  auto vertices_ = entity->vertices();
  for (int i = 0; i < vertices_.size(); i += 3) {
    glm::vec3 points[3];
    points[0] = vertices_[i];
    points[1] = vertices_[i + 1];
    points[2] = vertices_[i + 2];

    AABB aabb;
    aabb.min = glm::vec3(999999.0f, 999999.0f, 999999.0f);
    aabb.max = glm::vec3(-999999.0f, -999999.0f, -999999.0f);
    for (int j = 0; j < 3; j++) {
      if (points[j].x < aabb.min.x) aabb.min.x = points[j].x;
      if (points[j].x > aabb.max.x) aabb.max.x = points[j].x;
      if (points[j].y < aabb.min.y) aabb.min.y = points[j].y;
      if (points[j].y > aabb.max.y) aabb.max.y = points[j].y;
      if (points[j].z < aabb.min.z) aabb.min.z = points[j].z;
      if (points[j].z > aabb.max.z) aabb.max.z = points[j].z;
    }

    TestCollisionAABB(last_pos, aabb, points);
  }
}

void Physics::TestCollisionTerrain() {
  // float height = entity_manager_->GetTerrain()->GetHeight(player_->position().x, player_->position().z);
  float height = 0;
  if (player_->position().y - 45.0f < height) {
    glm::vec3 pos = player_->position();
    pos.y = height + 45.0f;
    player_->set_position(pos);
    glm::vec3 speed = player_->speed();
    if (speed.y < 0) speed.y = 0.0f;
    player_->set_speed(speed);
  }
}

void Physics::UpdateForces() {
  if (player_->over_ground()) {
    glm::vec3 speed = player_->speed();

    player_->set_speed(speed);
  } else {
    // player_->ApplyForce(glm::vec3(0, -0.2, 0)); 
  }
  player_->Update();
  player_->set_speed(glm::vec3(0, 0, 0));
  
  player_->set_over_ground(false);
  // TestCollision(player_->last_position(), entity_manager_->GetEntity("terrain"));
  TestCollisionTerrain();
}

} // End of namespace.
