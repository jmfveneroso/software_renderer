#ifndef _GEOMETRY_HPP_
#define _GEOMETRY_HPP_

#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp> 

namespace Sibyl {

enum PosRelativeToPlane {
  PLANE_FRONT,
  PLANE_BACK,
  PLANE_INTERSECT
};

struct Plane {
  glm::vec3 point;
  glm::vec3 vec1;
  glm::vec3 vec2;
  glm::vec3 normal;
};

struct Segment {
  bool null;
  glm::vec3 p1;
  glm::vec3 p2;
  Segment() : null(true) {}
  Segment(glm::vec3 p1, glm::vec3 p2) : null(false), p1(p1), p2(p2) {}
};

class Geometry {
 public:
  Geometry() {}

  static glm::vec3 GetSegPlaneIntersection(Segment seg, Plane plane);
  static PosRelativeToPlane GetPosRelativeToPlane(const Plane&, const glm::vec3&);
  static PosRelativeToPlane GetPosRelativeToPlane(const Plane&, const Segment&);
};

} // End of namespace.

#endif
