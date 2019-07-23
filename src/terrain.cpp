#include "terrain.hpp"

namespace Sibyl {

Terrain::Terrain(
  Shader shader, 
  Shader water_shader, 
  GLuint grass_texture_id, 
  GLuint sand_texture_id,
  GLuint water_diffuse_texture_id,
  GLuint water_normal_texture_id
) : shader_(shader),
    water_shader_(water_shader),
    grass_texture_id_(grass_texture_id), 
    sand_texture_id_(sand_texture_id),
    water_diffuse_texture_id_(water_diffuse_texture_id),
    water_normal_texture_id_(water_normal_texture_id) {

  height_map_ = new float[HEIGHT_MAP_SIZE * HEIGHT_MAP_SIZE];
  for (int x = 0; x < HEIGHT_MAP_SIZE; x++) {
    for (int y = 0; y < HEIGHT_MAP_SIZE; y++) {
      height_map_[y * HEIGHT_MAP_SIZE + x] = (sin(x * 0.01f) + sin(y * 0.01f)) - 0.5f;
    }
  }

  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i] = Clipmap(height_map_, i + 1); 
  }
}

void Terrain::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera, glm::vec3 player_pos) {
  glUseProgram(shader_.program_id());
  shader_.BindTexture("GrassTextureSampler", grass_texture_id_);
  shader_.BindTexture("SandTextureSampler", sand_texture_id_);

  // Clipmaps.
  for (int i = 0; i < CLIPMAP_LEVELS; i++) clipmaps_[i].Update(player_pos);

  bool first = true;
  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i].Render(player_pos, &shader_, ProjectionMatrix, ViewMatrix, first);
    first = false;
  }

  shader_.Clear();

  // Water.

  glUseProgram(water_shader_.program_id());
  water_shader_.BindTexture("dudvMap", water_diffuse_texture_id_);
  water_shader_.BindTexture("normalMap", water_normal_texture_id_);

  first = true;
  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i].RenderWater(player_pos, &water_shader_, ProjectionMatrix, ViewMatrix, camera, first);
    first = false;
  }

  water_shader_.Clear();
}

float Terrain::GetGridHeight(float x, float y) {
  int buffer_x = x / TILE_SIZE + HEIGHT_MAP_SIZE / 2;
  int buffer_y = y / TILE_SIZE + HEIGHT_MAP_SIZE / 2;

  float h = -4000.0f / MAX_HEIGHT;
  if (
    buffer_x < 0 || buffer_x >= HEIGHT_MAP_SIZE - 1 || 
    buffer_y < 0 || buffer_y >= HEIGHT_MAP_SIZE - 1
  ) {
    return h;
  }

  return height_map_[buffer_y * HEIGHT_MAP_SIZE + buffer_x];
}

float Terrain::GetHeight(float x , float y) { 
  int buffer_x = x / TILE_SIZE + HEIGHT_MAP_SIZE / 2;
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
