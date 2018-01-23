#ifndef _HEIGHT_MAP_HPP_
#define _HEIGHT_MAP_HPP_

#include <vector>
#include <iostream>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include "simplex_noise.hpp"
#include "config.h"

namespace Sibyl {

class HeightMap {
  SimplexNoise noise_;
  std::vector<glm::ivec2> feature_points_;

 public:
  HeightMap();
  float GetHeight(float x , float y);
};

} // End of namespace.

#endif
