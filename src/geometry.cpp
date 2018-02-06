#include "geometry.hpp"

namespace Sibyl {

glm::vec3 Geometry::GetSegPlaneIntersection(Segment seg, Plane plane) {
  glm::vec3 vec1 = plane.vec1 - plane.point;
  glm::vec3 vec2 = plane.vec2 - plane.point;
  glm::vec3 vec3 = seg.p2 - seg.p1;
  glm::vec3 vec4 = seg.p1 - plane.point;

  double denominator = glm::determinant(glm::mat3(vec1, vec2, vec3));
  if (fabs(denominator - 0.0f) < 0.0001) return glm::vec3(0, 0, 0);
  float alpha = -glm::determinant(glm::mat3(vec1, vec2, vec4)) / denominator;
  return seg.p1 + alpha * seg.p2;
}

PosRelativeToPlane Geometry::GetPosRelativeToPlane(
  const Plane& plane, const glm::vec3& p
) {
  if (glm::dot(plane.normal, p - plane.point) >= 0) {
    return PLANE_FRONT;
  } else {
    return PLANE_BACK;
  }
}

PosRelativeToPlane Geometry::GetPosRelativeToPlane(
  const Plane& plane, const Segment& seg
) {
  double dot_1 = glm::dot(plane.normal, seg.p1);
  double dot_2 = glm::dot(plane.normal, seg.p2);

  if (dot_1 > 0 && dot_2 > 0) {
    return PLANE_FRONT;
  } else if (dot_1 < 0 && dot_2 < 0) {
    return PLANE_BACK;
  } else {
    return PLANE_INTERSECT;
  }
}

} // End of namespace.
