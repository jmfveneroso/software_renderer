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

// glm::vec3 Geometry::GetSegPlaneIntersection(Segment seg, Plane plane) {
//   glm::vec3 vec1 = plane.vec1 - plane.point;
//   glm::vec3 vec2 = plane.vec2 - plane.point;
//   glm::vec3 vec3 = seg.p2 - seg.p1;
//   glm::vec3 vec4 = seg.p1 - plane.point;
// 
//   double denominator = glm::determinant(glm::mat3(vec1, vec2, vec3));
//   if (fabs(denominator - 0.0f) < 0.0001) return glm::vec3(0, 0, 0);
//   float alpha = -glm::determinant(glm::mat3(vec1, vec2, vec4)) / denominator;
//   return seg.p1 + alpha * seg.p2;
// }
// 
// PosRelativeToPlane Geometry::GetPosRelativeToPlane(
//   const Plane& plane, const glm::vec3& p
// ) {
//   if (glm::dot(plane.normal, p - plane.point) >= 0) {
//     return PLANE_FRONT;
//   } else {
//     return PLANE_BACK;
//   }
// }
// 
// PosRelativeToPlane Geometry::GetPosRelativeToPlane(
//   const Plane& plane, const Segment& seg
// ) {
//   double dot_1 = glm::dot(plane.normal, seg.p1);
//   double dot_2 = glm::dot(plane.normal, seg.p2);
// 
//   if (dot_1 > 0 && dot_2 > 0) {
//     return PLANE_FRONT;
//   } else if (dot_1 < 0 && dot_2 < 0) {
//     return PLANE_BACK;
//   } else {
//     return PLANE_INTERSECT;
//   }
// }

} // End of namespace.
