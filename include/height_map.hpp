#ifndef _HEIGHT_MAP_HPP_
#define _HEIGHT_MAP_HPP_

#include <fstream>
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

#define MAX_HEIGHT 10000
#define TILE_SIZE 128
#define HEIGHT_MAP_SIZE 5000
#define NUM_FEATURE_POINTS 12
#define PI 3.14159265359

namespace Sibyl {

class HeightMap {
  SimplexNoise noise_;
  std::vector<glm::ivec2> feature_points_;
  float* height_map_;
  float* secondary_height_map_;

  float Interpolate(float);
  void CreateHeightMap();
  void ApplyNoise();
  void ApplySmoothing();
  void ApplyPerturbationFilter();
  void ApplyRadialFilter();
  float GetNoise(float, float, float height = -4000.0f);
  float GetRadialFilter(float, float);

 public:
  HeightMap();
  float GetGridHeight(float, float);
  float GetHeight(float, float);
  void CalculateErosion();
  void Load(const std::string&);
  void Save(const std::string&);
};

} // End of namespace.

#endif
