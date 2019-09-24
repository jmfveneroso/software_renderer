#include "building.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

Building::Building(shared_ptr<Renderer> renderer) : renderer_(renderer) {
  float s = 14.0f;
  float t = 0.25f;
  CreateFloor(vec3(1995, 205,   1995), s, true );
  CreateFloor(vec3(1995, 211.5, 1995), s, false);
  CreateFloor(vec3(1995, 218,   1995), s, false);

  vec3 pos = vec3(1995, 205, 1995);
  floors_.push_back(Floor(pos + vec3(-t,  19.5, -t  ), s+1+2*t, 1, t  ));
  floors_.push_back(Floor(pos + vec3(s+1, 19.5, 0   ), t,       1, s+1));
  floors_.push_back(Floor(pos + vec3(-t,  19.5, s+1 ), s+1+2*t, 1, t  ));
  floors_.push_back(Floor(pos + vec3(-t,  19.5, 0   ), t,       1, s+1));

  floors_.push_back(Floor(pos + vec3(4.25,  0, 2), 6.75, 1, 0.5));
  floors_.push_back(Floor(pos + vec3(4,  0, 2), 0.25, 4, 13));

  floors_.push_back(Floor(pos + vec3(14, 0, 5), 1, 1, 9));
}

void Building::CreateFloor(glm::vec3 position, float s, bool door) {
  float t = 0.25;

  if (door) {
    floors_.push_back(Floor(position + vec3(-t,    0, -t), s/2+t, 6, t));
    floors_.push_back(Floor(position + vec3(s/2+1, 0, -t), s/2+t, 6, t));
    floors_.push_back(Floor(position + vec3(s/2,   2, -t), 1,     4, t));
  } else {
    floors_.push_back(Floor(position + vec3(-t, 0, -t), s+1+2*t, 6, t));
  }

  floors_.push_back(Floor(position + vec3(s+1, 0, 0),     t, 6, s/2));
  floors_.push_back(Floor(position + vec3(s+1, 0, s/2+1), t, 6, s/2));
  floors_.push_back(Floor(position + vec3(s+1, 4, s/2),   t, 2, 1  ));
  floors_.push_back(Floor(position + vec3(s+1, 0, s/2),   t, 1, 1  ));

  floors_.push_back(Floor(position + vec3(-t, 0, s+1), s+1+2*t, 6, t));

  floors_.push_back(Floor(position + vec3(-t, 0, 0    ), t, 6, s/2));
  floors_.push_back(Floor(position + vec3(-t, 0, s/2+1), t, 6, s/2));
  floors_.push_back(Floor(position + vec3(-t, 4, s/2  ), t, 2, 1  ));
  floors_.push_back(Floor(position + vec3(-t, 0, s/2  ), t, 1, 1  ));

  floors_.push_back(Floor(position + vec3(-t,    6, -t   ), s+1+2*t, 0.5, s-1+t));
  floors_.push_back(Floor(position + vec3(-t,    6, s-1-t), s-1-6+t, 0.5, 2+2*t));
  floors_.push_back(Floor(position + vec3(s-1-6, 6, s-1+2), 6,       0.5, t    ));
  floors_.push_back(Floor(position + vec3(s-1,   6, s-1  ), 2+t,     0.5, 2+t  ));
 
  // Stairs.
  float x = 0;
  for (int i = 0; i < 12; i++) {
    x += 0.5;
    floors_.push_back(Floor(position + vec3(s-1 - x, 6 - x, s-1), 0.5, 0.5, 2));
  }
}

void Building::CollideFloor(Floor& f, glm::vec3& player_pos, glm::vec3 prev_pos, bool& can_jump, glm::vec3& speed, BoundingBox& p) {
  // AABB collision.
  if (
      p.x >= f.position.x + f.width  || p.x + p.width  <= f.position.x ||
      p.y >= f.position.y + f.height || p.y + p.height <= f.position.y ||
      p.z >= f.position.z + f.length || p.z + p.length <= f.position.z
  ) return;

  // Collision vector.
  vec3 collision = player_pos - prev_pos;

  // Collision point between the player and each box face in collision 
  // vector coordinates.
  vector<double> collision_magnitude {
    (f.position.y + f.height + 1.50 - prev_pos.y) / collision.y, // Top.
    (f.position.y - prev_pos.y                 )  / collision.y, // Bottom.
    (f.position.x + f.width  + 0.35 - prev_pos.x) / collision.x, // Right.
    (f.position.x - 0.35 - prev_pos.x          )  / collision.x, // Left.
    (f.position.z + f.length + 0.35 - prev_pos.z) / collision.z, // Front.
    (f.position.z - 0.35 - prev_pos.z          )  / collision.z  // Back.
  };

  // Select the face for which the collision displacement is minimum
  // when moving the player only along the collision vector.
  int min_index = -1;
  double min_value = numeric_limits<double>::max();
  for (int i = 0; i < 6; i++) {
    double k = collision_magnitude[i];
    if (k < 0.0 || k > 1.0) continue;

    if (min_index == -1 || k < min_value) {
      min_index = i;
      min_value = k;
    }
  }

  // Displace the player such that it is no longer in collision with
  // the selected face.
  switch (min_index) {
    case 0:
      can_jump = true;
      speed.y = 0;
    case 1:
      player_pos.y = prev_pos.y + min_value * collision.y;
      player_pos.y += ((min_index % 2) ? -0.0001 : 0.0001);
      break;
    case 2:
    case 3:
      player_pos.x = prev_pos.x + min_value * collision.x;
      player_pos.x += ((min_index % 2) ? -0.0001 : 0.0001);
      break;
    case 4:
    case 5:
      player_pos.z = prev_pos.z + min_value * collision.z;
      player_pos.z += ((min_index % 2) ? -0.0001 : 0.0001);
      break;
    default:
      break;
  }
}

void Building::Collide(glm::vec3& player_pos, glm::vec3 prev_pos, bool& can_jump, glm::vec3& speed, BoundingBox& p) {
  for (auto& f : floors_) {
    CollideFloor(f, player_pos, prev_pos, can_jump, speed, p);
  }
}

void Building::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  for (auto& f : floors_) {
    vec3 dimensions(f.width, f.length, f.height);
    renderer_->DrawCube(ProjectionMatrix, ViewMatrix, camera, f.position, dimensions, 0.0);
  }
}

} // End of namespace.
