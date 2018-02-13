#include "geometry.hpp"

namespace Sibyl {

std::vector<glm::vec3> Geometry::ClipPlane(
  const std::vector<glm::vec3>& points, const Plane& plane
) {
  if (points.size() < 2) return std::vector<glm::vec3>();

  std::vector<glm::vec3> result;
  glm::vec3 last_point = points.back();
  float last_distance = glm::dot(plane.normal, (last_point - plane.point));
  for (int i = 0; i < points.size(); i++) {
    float distance = glm::dot(plane.normal, (points[i] - plane.point));

    // Both in front of plane.
    if (last_distance > 0.0f && distance > 0.0f) {
      result.push_back(points[i]); 

    // Last in front and current behind plane.
    } else if (last_distance > 0.0f && distance <= 0.0f) {
      float alpha = -last_distance / (distance - last_distance);
      glm::vec3 intersection = last_point * (1 - alpha) + points[i] * alpha;
      result.push_back(intersection); 

    // Last behind and current in front of plane.
    } else if (last_distance <= 0.0f && distance > 0.0f) {
      float alpha = -last_distance / (distance - last_distance);
      glm::vec3 intersection = last_point * (1 - alpha) + points[i] * alpha;
      result.push_back(intersection); 
      result.push_back(points[i]); 
    }
    last_point = points[i];
    last_distance = distance;
  }
  return result;
}

void Geometry::FlattenPolygon(std::vector<glm::vec3>& points, const Plane& plane) {
  for (int i = 0; i < points.size(); i++) {
    float distance = glm::dot(plane.normal, (points[i] - plane.point));
    if (distance < 0.0) {
      points[i] -= plane.normal * distance;
    }
  }
}

} // End of namespace.
