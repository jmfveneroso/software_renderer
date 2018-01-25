#include "height_map.hpp"

namespace Sibyl {

HeightMap::HeightMap() {
  feature_points_.reserve(10);
  for (int i = 0; i < 10; i++) {
    int x = -HEIGHT_MAP_SIZE/2 * TILE_SIZE + rand() % (HEIGHT_MAP_SIZE * TILE_SIZE);
    int y = -HEIGHT_MAP_SIZE/2 * TILE_SIZE + rand() % (HEIGHT_MAP_SIZE * TILE_SIZE); 
    feature_points_[i] = glm::ivec2(x, y);
  }

  CreateHeightMap();
}

float HeightMap::Interpolate(float height) {
  return height / MAX_HEIGHT;
}

void HeightMap::CreateHeightMap() {
  height_map_ = new float[HEIGHT_MAP_SIZE * HEIGHT_MAP_SIZE];
  secondary_height_map_ = new float[HEIGHT_MAP_SIZE * HEIGHT_MAP_SIZE];
  for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
    for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
      glm::ivec2 world_coords = glm::ivec2((x - HEIGHT_MAP_SIZE / 2) * TILE_SIZE, (y - HEIGHT_MAP_SIZE / 2) * TILE_SIZE);

      // world_coords.x = world_coords.x + HEIGHT_MAP_SIZE * TILE_SIZE * 0.1f * noise_.noise(0.0005 * x, 0.0005 * y);
      // world_coords.y = world_coords.y + HEIGHT_MAP_SIZE * TILE_SIZE * 0.1f * noise_.noise(0.0005 * x, 0.0005 * y);
      // int actual_y = y + 0.01f * noise_.noise(1000 + 0.001 * x, 1000 + 0.001 * y) * HEIGHT_MAP_SIZE;
      // if (actual_x < 0 || actual_x >= HEIGHT_MAP_SIZE || actual_y < 0 || actual_y >= HEIGHT_MAP_SIZE) continue;
      // height_map_[y * HEIGHT_MAP_SIZE + x] = height_map_[actual_y * HEIGHT_MAP_SIZE + actual_x];

      int point_1 = -1;
      int point_2 = -1;
      int point_3 = -1;
      float distance_1 = 1000000000;
      float distance_2 = 1000000000;
      float distance_3 = 1000000000;
      for (int i = 0; i < 10; i++) {
        float distance = sqrt(pow(feature_points_[i].x - world_coords.x, 2) + pow(feature_points_[i].y - world_coords.y, 2)); 
        if (distance < distance_1) {
          distance_1 = distance;
          point_1 = i;
        }
      }

      for (int i = 0; i < 10; i++) {
        if (i == point_1) continue;
        float distance = sqrt(pow(feature_points_[i].x - world_coords.x, 2) + pow(feature_points_[i].y - world_coords.y, 2)); 
        if (distance < distance_2) {
          distance_2 = distance;
          point_2 = i;
        }
      }

      for (int i = 0; i < 10; i++) {
        if (i == point_1) continue;
        if (i == point_2) continue;
        float distance = sqrt(pow(feature_points_[i].x - world_coords.x, 2) + pow(feature_points_[i].y - world_coords.y, 2)); 
        if (distance < distance_3) {
          distance_3 = distance;
          point_3 = i;
        }
      }

      float h = 0;
      h = -10000 * log(64 + distance_2 - distance_1);

      // float world_x = world_coords.x;
      // float world_y = world_coords.y;
      // h += 5 * (
      //   2000 * noise_.noise(world_x * 0.00002, world_y * 0.00002) +
      //   1500 * noise_.noise((1000 + world_x) * 0.00002, (1000 + world_y) * 0.00002) +
      //   510 * noise_.noise(world_x * 0.0001, world_y * 0.0001) +
      //   40 * noise_.noise(world_x * 0.001, world_y * 0.001) +
      //   0
      // );

      // height_map_[y * HEIGHT_MAP_SIZE + x] = Interpolate(h);
      height_map_[y * HEIGHT_MAP_SIZE + x] = h;
    }
  }

  for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
    for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
      glm::ivec2 world_coords = glm::ivec2((x - HEIGHT_MAP_SIZE / 2) * TILE_SIZE, (y - HEIGHT_MAP_SIZE / 2) * TILE_SIZE);
      float world_x = world_coords.x;
      float world_y = world_coords.y;
      float h = height_map_[y * HEIGHT_MAP_SIZE + x];
      h += 0.3f * (
        2000 * noise_.noise(world_x * 0.00002, world_y * 0.00002) +
        1500 * noise_.noise((1000 + world_x) * 0.00002, (1000 + world_y) * 0.00002) +
        510 * noise_.noise(world_x * 0.0001, world_y * 0.0001) +
        40 * noise_.noise(world_x * 0.001, world_y * 0.001) +
        0
      );
      height_map_[y * HEIGHT_MAP_SIZE + x] = h;
    }
  }
 
  ApplyPerturbationFilter();

  // CalculateErosion();

  for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
    for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
      height_map_[y * HEIGHT_MAP_SIZE + x] = Interpolate(height_map_[y * HEIGHT_MAP_SIZE + x]);
    }
  }
}

void HeightMap::ApplyPerturbationFilter() {
  int bla = 0;
  for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
    for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
      float tex_x = float(x) / 300;
      float tex_y = float(y) / 300;
      // float tex_x = 0.001 * float(x);
      // float tex_y = 0.001 * float(y);

      int perturbed_x = x + 0.005f * noise_.noise(tex_x        , tex_y        ) * HEIGHT_MAP_SIZE;
      int perturbed_y = y + 0.005f * noise_.noise(tex_x + 1.5f, tex_y + 1.5f) * HEIGHT_MAP_SIZE;
      // int perturbed_x = x + int(50.0f * float(sin(3.14 + tex_x*6.28) + sin(3.14 + tex_y*6.28)));
      // int perturbed_y = y + int(50.0f * float(sin(tex_x*6.28) + sin(tex_y*6.28)));

      // if (perturbed_x < 0) perturbed_x = HEIGHT_MAP_SIZE + perturbed_x;
      // if (perturbed_y < 0) perturbed_y = HEIGHT_MAP_SIZE + perturbed_y;
      // std::cout << "x:  " << x           << " y:   " << y           << std::endl;
      // std::cout << "p_x:" << perturbed_x << " p_y: " << perturbed_y << std::endl;

      if (perturbed_x < 0 || perturbed_x >= HEIGHT_MAP_SIZE) {
        secondary_height_map_[y * HEIGHT_MAP_SIZE + x] = 0;
        continue;
      }
      if (perturbed_y < 0 || perturbed_y >= HEIGHT_MAP_SIZE) {
        secondary_height_map_[y * HEIGHT_MAP_SIZE + x] = 0;
        continue;
      }

      // perturbed_x = perturbed_x % HEIGHT_MAP_SIZE;
      // perturbed_y = perturbed_y % HEIGHT_MAP_SIZE;

      secondary_height_map_[y * HEIGHT_MAP_SIZE + x] = height_map_[perturbed_y * HEIGHT_MAP_SIZE + perturbed_x];
    }
  }

  for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
    for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
      height_map_[y * HEIGHT_MAP_SIZE + x] = secondary_height_map_[y * HEIGHT_MAP_SIZE + x];
    }
  }
}

void HeightMap::CalculateErosion() {
  float c = 0.5;
  // float tallus = 0.0002;
  float tallus = 0.00002;
  for (int i = 0; i < 10; i++) {
    std::cout << "wtf: " << i << std::endl;
    for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
      for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
        secondary_height_map_[y * HEIGHT_MAP_SIZE + x] = height_map_[y * HEIGHT_MAP_SIZE + x];
      }
    }

    for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
      for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
        if (x == 0 || x == HEIGHT_MAP_SIZE - 1 || y == 0 || y == HEIGHT_MAP_SIZE - 1) continue;
        float h = height_map_[y * HEIGHT_MAP_SIZE + x];
        float d_total = 0;
        float d_max = 0;
        int max_m = 0; int max_n = 0;

        for (int m = -1; m <= 1; m++) {
          for (int n = -1; n <= 1; n++) {
            if (m == n == 0) continue;
            float h_i = height_map_[(y + n) * HEIGHT_MAP_SIZE + (x + m)];
            float d_i = h - h_i;
            if (d_i > tallus) {
              if (d_i > d_max) {
                d_max = d_i;
                max_m = m;
                max_n = n;
              }

              d_total += d_i;
            }
          }
        }

        // if (max_m == max_n == 0) continue;

        // float material = d_max / 2;
        // eroded_height_map_[y * HEIGHT_MAP_SIZE + x] -= material;
        // eroded_height_map_[(y + max_n) * HEIGHT_MAP_SIZE + (x + max_m)] += material;

        for (int m = -1; m <= 1; m++) {
          for (int n = -1; n <= 1; n++) {
            if (m == n == 0) continue;
            float h_i = height_map_[(y + n) * HEIGHT_MAP_SIZE + (x + m)];
            float d_i = h - h_i;
            if (d_i > tallus) {
              float material = c * (d_max - tallus) * (d_i / d_total);
              secondary_height_map_[y * HEIGHT_MAP_SIZE + x] -= material;
              secondary_height_map_[(y + n) * HEIGHT_MAP_SIZE + (x + m)] += material;
            }
          }
        }
      }
    }

    for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
      for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
        height_map_[y * HEIGHT_MAP_SIZE + x] = secondary_height_map_[y * HEIGHT_MAP_SIZE + x];
      }
    }
  }
}

float HeightMap::GetHeight(float x, float y) {
  int buffer_x = x / TILE_SIZE + HEIGHT_MAP_SIZE / 2;
  int buffer_y = y / TILE_SIZE + HEIGHT_MAP_SIZE / 2;
  // std::cout << "x:  " << buffer_x           << " y:   " << buffer_y           << std::endl;
  // buffer_x = buffer_x % HEIGHT_MAP_SIZE;
  // buffer_y = buffer_y % HEIGHT_MAP_SIZE;
  // if (buffer_x < 0 || buffer_y < 0) return 0;

  if (buffer_x < 0 || buffer_x >= HEIGHT_MAP_SIZE - 1|| buffer_y < 0 || buffer_y >= HEIGHT_MAP_SIZE - 1)
    return 0;
  //   return Interpolate(2 * (
  //     2450 * noise_.noise(x * 0.00002, y * 0.00002) +
  //     1500 * noise_.noise((1000 + x) * 0.00002, (1000 + y) * 0.00002) +
  //     510 * noise_.noise(x * 0.0001, y * 0.0001) +
  //     40 * noise_.noise(x * 0.001, y * 0.001) +
  //     0
  //   ));
  // }  

  float h = height_map_[buffer_y * HEIGHT_MAP_SIZE + buffer_x];
  return h;
}

} // End of namespace.
