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

/*
Returns the point intersection between a ray (defined by a point and a direction)
and a floor. The floor is aligned with the grid, so each of the six planes can
be described by an equation such as X = 10, Y = 5 or Z = 15. Therefore, the
scaling factor k, that determines the position of the intersection along the 
direction vector for a plane with equation X = 1O can be calculated with:

point.x + direction.x * k = 10
k = (10 - point.x) / direction.x
vec3 intersection = point + direction * k

*/
PointIntersection Building::GetPointIntersection(Floor& f, vec3 point, vec3 direction) {
  // The direction must be a unit vector. 
  direction /= length(direction); 

  vec3 dimensions(f.width, f.height, f.length);
  
  // (LBB) Left, Bottom, Back plane equations.
  vec3 lbb = f.position;          

  // (RTF) Right, Top, Front plane equations.
  vec3 rtf = lbb + dimensions;

  // Calculate intersection scaling factor k.
  vec3 k_lbb = (lbb - point) / direction;
  vec3 k_rtf = (rtf - point) / direction;
  float k[6] = { k_lbb.x, k_lbb.y, k_lbb.z, k_rtf.x, k_rtf.y, k_rtf.z };

  vector<vec3> normals = { 
    vec3(-1, 0, 0), vec3(0, -1, 0), vec3(0, 0, -1),
    vec3(+1, 0, 0), vec3(0, +1, 0), vec3(0, 0, +1) 
  };

  bool intersection_found = false;
  vec3 normal;
  GLfloat min_distance = numeric_limits<GLfloat>::max();
  for (int i = 0; i < 6; i++) {
    if (k[i] < 0.0) continue; 

    vec3 overlap = (point + direction * k[i]) - lbb;
    if (any(lessThan(overlap, vec3(0)))) continue;
    if (any(greaterThan(overlap, dimensions))) continue;

    if (k[i] < min_distance) {
      min_distance = k[i];
      normal = normals[i];
      intersection_found = true;
    }
  }

  if (intersection_found) {
    return { true, point + direction * min_distance, normal, min_distance };
  } else {
    return { false, vec3(0, 0, 0), vec3(0, 0, 0), numeric_limits<GLfloat>::max() };
  }
}

PointIntersection Building::GetPointIntersection(vec3 point, vec3 direction) {
  PointIntersection p;
  for (auto& f : floors_) {
    PointIntersection aux = GetPointIntersection(f, point, direction);
    if (!aux.valid) continue;
    if (aux.distance < p.distance) p = aux;
  }
  return p;
}

void Building::Collide(glm::vec3& player_pos, glm::vec3 prev_pos, bool& can_jump, glm::vec3& speed, BoundingBox& p) {
  for (auto& f : floors_) {
    CollideFloor(f, player_pos, prev_pos, can_jump, speed, p);
  }
}

void Building::DryCollide(vec3& pos, BoundingBox& p) {
  for (auto& f : floors_) {
    // AABB collision.
    if (
        p.x >= f.position.x + f.width  || p.x + p.width  <= f.position.x ||
        p.y >= f.position.y + f.height || p.y + p.height <= f.position.y ||
        p.z >= f.position.z + f.length || p.z + p.length <= f.position.z
    ) continue;

    // Collision vector.
    vec3 collision = vec3(pos.x + p.width/2, pos.y + p.height/2, pos.z + p.length/2);
    collision -= f.position + vec3(f.width, f.height, f.length);

    vector<GLfloat> components {
      (collision.y > 0) ? collision.y : numeric_limits<GLfloat>::max(),
      (collision.y < 0) ? -collision.y : numeric_limits<GLfloat>::max(),
      (collision.x > 0) ? collision.x : numeric_limits<GLfloat>::max(),
      (collision.x < 0) ? -collision.x : numeric_limits<GLfloat>::max(),
      (collision.z > 0) ? collision.z : numeric_limits<GLfloat>::max(),
      (collision.z < 0) ? -collision.z : numeric_limits<GLfloat>::max(),
    };

    int min_index = -1;
    GLfloat minimum = numeric_limits<GLfloat>::max();
    for (int i = 0; i < components.size(); i++) {
      if (components[i] < minimum) {
        minimum = components[i];
        min_index = i;
      }
    }

    switch (min_index) {
      case 0: // Top.
        pos.y = f.position.y + f.height;
        break;
      case 1: // Bottom.
        pos.y = f.position.y - p.height;
        break;
      case 2: // Right.
        pos.x = f.position.x + f.width;
        break;
      case 3: // Left.
        pos.x = f.position.x - p.width;
        break;
      case 4: // Front.
        pos.z = f.position.z + f.length;
        break;
      case 5: // Back.
        pos.z = f.position.z - p.length;
        break;
    } 
  }
}

void Building::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  for (auto& f : floors_) {
    vec3 dimensions(f.width, f.length, f.height);
    renderer_->DrawCube(ProjectionMatrix, ViewMatrix, camera, f.position, dimensions, 0.0);
  }
}

} // End of namespace.
