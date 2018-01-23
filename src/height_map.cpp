#include "height_map.hpp"

namespace Sibyl {

HeightMap::HeightMap() {
  feature_points_.reserve(20);
  for (int i = 0; i < 20; i++) {
    int x = -500 * 64 + rand() % (1000 * 64);
    int y = -500 * 64 + rand() % (1000 * 64); 
    feature_points_[i] = glm::ivec2(x, y);
  }
}

float HeightMap::GetHeight(float x, float y) {
  glm::ivec2 world_coords = glm::ivec2(x, y);

  int point_1 = -1;
  int point_2 = -1;
  int point_3 = -1;
  float distance_1 = 1000000000;
  float distance_2 = 1000000000;
  float distance_3 = 1000000000;
  for (int i = 0; i < 20; i++) {
    float distance = sqrt(pow(feature_points_[i].x - world_coords.x, 2) + pow(feature_points_[i].y - world_coords.y, 2)); 
    if (distance < distance_1) {
      distance_1 = distance;
      point_1 = i;
    }
  }

  for (int i = 0; i < 20; i++) {
    if (i == point_1) continue;
    float distance = sqrt(pow(feature_points_[i].x - world_coords.x, 2) + pow(feature_points_[i].y - world_coords.y, 2)); 
    if (distance < distance_2) {
      distance_2 = distance;
      point_2 = i;
    }
  }

  for (int i = 0; i < 20; i++) {
    if (i == point_1) continue;
    if (i == point_2) continue;
    float distance = sqrt(pow(feature_points_[i].x - world_coords.x, 2) + pow(feature_points_[i].y - world_coords.y, 2)); 
    if (distance < distance_3) {
      distance_3 = distance;
      point_3 = i;
    }
  }

  float h = distance_1 - distance_2;
  // std::cout << "distance_1: " << distance_1 << std::endl;
  // std::cout << "distance_2: " << distance_2 << std::endl;

  // h = 0.4 * h + 0.6 * (
  //   2450 * noise_.noise(x * 0.00002, y * 0.00002) +
  //   1500 * noise_.noise((1000 + x) * 0.00002, (1000 + y) * 0.00002) +
  //   510 * noise_.noise(x * 0.0001, y * 0.0001) +
  //   40 * noise_.noise(x * 0.001, y * 0.001) +
  //   0
  // )/ 4000.0f;

  h = h / 256000;
  // h = float((h + 256000)) / 512000;
  // std::cout << "h: " << h << std::endl;
  return h;
}

} // End of namespace.
