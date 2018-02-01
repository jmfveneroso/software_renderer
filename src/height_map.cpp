#include "height_map.hpp"

namespace Sibyl {

HeightMap::HeightMap() {
  feature_points_.reserve(NUM_FEATURE_POINTS);
  for (int i = 0; i < NUM_FEATURE_POINTS; i++) {
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
  Load("assets/height_maps/small_height_map.data");
  return;

  secondary_height_map_ = new float[HEIGHT_MAP_SIZE * HEIGHT_MAP_SIZE];
  for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
    for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
      glm::ivec2 world_coords = glm::ivec2((x - HEIGHT_MAP_SIZE / 2) * TILE_SIZE, (y - HEIGHT_MAP_SIZE / 2) * TILE_SIZE);

      int point_1 = -1;
      int point_2 = -1;
      float distance_1 = 1000000000;
      float distance_2 = 1000000000;
      for (int i = 0; i < NUM_FEATURE_POINTS; i++) {
        float distance = sqrt(pow(feature_points_[i].x - world_coords.x, 2) + pow(feature_points_[i].y - world_coords.y, 2)); 
        if (distance < distance_1) {
          distance_1 = distance;
          point_1 = i;
        }
      }

      for (int i = 0; i < NUM_FEATURE_POINTS; i++) {
        if (i == point_1) continue;
        float distance = sqrt(pow(feature_points_[i].x - world_coords.x, 2) + pow(feature_points_[i].y - world_coords.y, 2)); 
        if (distance < distance_2) {
          distance_2 = distance;
          point_2 = i;
        }
      }

      float h = 0;

      float initial_height = 15000;
      h = 1.5f * (initial_height -distance_1 + distance_2);
      if (h < initial_height + 5000) {
        h = initial_height + 5000;
      }

      // if (h < initial_height + 2000) {
      //   h = initial_height + 2000 + -100 * log(1 + (initial_height + 2000 - h) / 100);
      // }

      height_map_[y * HEIGHT_MAP_SIZE + x] = h;
    }
  }
 
  ApplyNoise();
  ApplyPerturbationFilter();
  ApplySmoothing();
  ApplyNoise();

  for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
    for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
      height_map_[y * HEIGHT_MAP_SIZE + x] = Interpolate(height_map_[y * HEIGHT_MAP_SIZE + x]);
    }
  }

  CalculateErosion();
  Save("assets/height_maps/small_height_map.data");
}

float HeightMap::GetNoise(float world_x, float world_y, float height) {
  return height + 1.5f * (
    2000 * noise_.noise(world_x * 0.00002, world_y * 0.00002) +
    // 1500 * noise_.noise((1000 + world_x) * 0.00002, (1000 + world_y) * 0.00002) +
    // 200 * noise_.noise(world_x * 0.0003, world_y * 0.0003) +
    100 * noise_.noise(world_x * 0.0006, world_y * 0.0006) +
    0
  );
}

float HeightMap::GetRadialFilter(float x, float y) {
  float distance = sqrt(pow(x, 2) + pow(y, 2)); 
  float alpha = 1 - (distance / (TILE_SIZE * HEIGHT_MAP_SIZE / 2));
  return (alpha > 0) ? 0.25 * sin(alpha * PI / 2) : 0;
}

void HeightMap::ApplyNoise() {
  for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
    for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
      glm::ivec2 world_coords = glm::ivec2((x - HEIGHT_MAP_SIZE / 2) * TILE_SIZE, (y - HEIGHT_MAP_SIZE / 2) * TILE_SIZE);
      float world_x = world_coords.x;
      float world_y = world_coords.y;
      float h = height_map_[y * HEIGHT_MAP_SIZE + x];
      h += GetNoise(world_x, world_y);
      height_map_[y * HEIGHT_MAP_SIZE + x] = h;
    }
  }
}

void HeightMap::ApplySmoothing() {
  for (int i = 0; i < 2; i++) {
    for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
      for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
        if (x == 0 || x == HEIGHT_MAP_SIZE - 1 || y == 0 || y == HEIGHT_MAP_SIZE - 1) continue;

        float h = 0;
        for (int m = -1; m <= 1; m++) {
          for (int n = -1; n <= 1; n++) {
            h += (1.0f / 9) * height_map_[(y + n) * HEIGHT_MAP_SIZE + (x + m)];
          }
        }
        height_map_[y * HEIGHT_MAP_SIZE + x] = h;
      }
    }
  }
}

void HeightMap::ApplyPerturbationFilter() {
  int bla = 0;
  for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
    for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
      float tex_x = 1.5f * float(x) / HEIGHT_MAP_SIZE;
      float tex_y = 1.5f * float(y) / HEIGHT_MAP_SIZE;

      int perturbed_x = x + 0.25f * noise_.noise(tex_x        , tex_y        ) * HEIGHT_MAP_SIZE;
      int perturbed_y = y + 0.25f * noise_.noise(tex_x + 1.5f, tex_y + 1.5f) * HEIGHT_MAP_SIZE;

      perturbed_x = perturbed_x + 0.02f * noise_.noise(10 * tex_x        , 10 * tex_y        ) * HEIGHT_MAP_SIZE;
      perturbed_y = perturbed_y + 0.02f * noise_.noise(10 * tex_x + 1.5f , 10 * tex_y + 1.5f) * HEIGHT_MAP_SIZE;

      if (perturbed_x < 0) perturbed_x = -perturbed_x;
      if (perturbed_y < 0) perturbed_y = -perturbed_y;

      if (perturbed_x >= HEIGHT_MAP_SIZE) perturbed_x = 2 * HEIGHT_MAP_SIZE - 1 - perturbed_x;
      if (perturbed_y >= HEIGHT_MAP_SIZE) perturbed_y = 2 * HEIGHT_MAP_SIZE - 1 - perturbed_y;

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
  float tallus = 0.0005;
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

void HeightMap::Load(const std::string& filename) {
  std::ifstream is;  
  is.open(filename, std::ios::binary);
  for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
    for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
      is.read((char*) &height_map_[y * HEIGHT_MAP_SIZE + x], sizeof(float));
    }
  }
  is.close();
}

void HeightMap::Save(const std::string& filename) {
  std::ofstream os;  
  os.open(filename, std::ios::binary);
  for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
    for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
      os.write((char*) &height_map_[y * HEIGHT_MAP_SIZE + x], sizeof(float));
    }
  }
  os.close();
}

float HeightMap::GetGridHeight(float x, float y) {
  int buffer_x = x / TILE_SIZE + HEIGHT_MAP_SIZE / 2;
  int buffer_y = y / TILE_SIZE + HEIGHT_MAP_SIZE / 2;

  float h = Interpolate(GetNoise(x, y));
  if (
    buffer_x < 0 || buffer_x >= HEIGHT_MAP_SIZE - 1 || 
    buffer_y < 0 || buffer_y >= HEIGHT_MAP_SIZE - 1
  ) {
    return h;
  }

  float alpha = GetRadialFilter(x, y);
  return alpha * height_map_[buffer_y * HEIGHT_MAP_SIZE + buffer_x] + (1.0f - alpha) * h;
}

float HeightMap::GetHeight(float x, float y) {
  // Clamp to grid.
  glm::ivec2 top_left = (glm::ivec2(x, y) / TILE_SIZE) * TILE_SIZE;
  if (x < 0 && fabs(top_left.x - x) > 0.00001) top_left.x -= TILE_SIZE;
  if (y < 0 && fabs(top_left.y - y) > 0.00001) top_left.y -= TILE_SIZE;

  float v[4];
  v[0] = GetGridHeight(top_left.x                  , top_left.y                  );
  v[1] = GetGridHeight(top_left.x                  , top_left.y + TILE_SIZE + 0.1);
  v[2] = GetGridHeight(top_left.x + TILE_SIZE + 0.1, top_left.y + TILE_SIZE + 0.1);
  v[3] = GetGridHeight(top_left.x + TILE_SIZE + 0.1, top_left.y                  );

  glm::vec2 tile_v = (glm::vec2(x, y) - glm::vec2(top_left)) / float(TILE_SIZE);

  // Top triangle.
  float h;
  if (tile_v.x + tile_v.y < 1.0f) {
    return v[0] + tile_v.x * (v[3] - v[0]) + tile_v.y * (v[1] - v[0]);

  // Bottom triangle.
  } else {
    tile_v = glm::vec2(1.0f) - tile_v; 
    return v[2] + tile_v.x * (v[1] - v[2]) + tile_v.y * (v[3] - v[2]);
  }
}

} // End of namespace.
