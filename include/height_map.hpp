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

#define MAX_HEIGHT 256000
#define TILE_SIZE 64
#define HEIGHT_MAP_SIZE 5000

namespace Sibyl {

class HeightMap {
  SimplexNoise noise_;
  std::vector<glm::ivec2> feature_points_;
  float* height_map_;
  float* secondary_height_map_;

  float Interpolate(float);
  void CreateHeightMap();
  void ApplyPerturbationFilter();

 public:
  HeightMap();
  float GetHeight(float, float);
  void CalculateErosion();
};

} // End of namespace.

#endif
