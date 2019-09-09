#include "terrain.hpp"

using namespace std;

namespace Sibyl {

Terrain::Terrain(
  GLuint grass_texture_id, 
  GLuint sand_texture_id,
  GLuint water_diffuse_texture_id,
  GLuint water_normal_texture_id
) : shader_("terrain", "v_terrain", "f_terrain", "g_terrain"),
    water_shader_("water", "v_water", "f_water"),
    grass_texture_id_(grass_texture_id), 
    sand_texture_id_(sand_texture_id),
    water_diffuse_texture_id_(water_diffuse_texture_id),
    water_normal_texture_id_(water_normal_texture_id) {

  LoadTerrain("./meshes/terrain.data");

  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i] = Clipmap(height_map_, i + 1); 
  }
}

void Terrain::LoadTerrain(const string& filename) {
  ifstream is(filename, ifstream::binary);
  if (!is) return;

  int size;
  is >> size;

  // The map is 41 X 41, where each tile is 5 x 5 meters wide.
  // But in our actual map, each tile is 1 x 1 meters wide.
  vector< vector<float> > height_data(size+1, vector<float>(size+1, 0.0));

  float height;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      is >> height_data[i][j];
    }
  }
  is.close();

  // The step is the gap between sample points in the map grid.
  int step = 5;
  size = (size-1) * step + 1;
  height_map_ = vector< vector<float> >(size, vector<float>(size, 0.0));

  // Now we need to do linear interpolation to obtain the 1 x 1 meter wide
  // tile heights.
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      int grid_x = i / step;
      int grid_y = j / step;
      int offset_x = i % step;
      int offset_y = j % step;

      float top_lft = height_data[grid_x][grid_y];
      float top_rgt = height_data[grid_x+1][grid_y];
      float bot_lft = height_data[grid_x][grid_y+1];
      float bot_rgt = height_data[grid_x+1][grid_y+1];

      float height = 0.0;
      height_map_[i][j] = top_lft;
  
      // Top left triangle.
      if (offset_x + offset_y <= step) {
        height = top_lft;
        height += (top_rgt - top_lft) * (offset_x / float(step));
        height += (bot_lft - top_lft) * (offset_y / float(step));

      // Bottom right triangle.
      } else {
        height = bot_rgt;
        height += (bot_lft - bot_rgt) * (1 - (offset_x / float(step)));
        height += (top_rgt - bot_rgt) * (1 - (offset_y / float(step)));
      }

      height_map_[i][j] = height;
    }
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

float Terrain::GetHeight(float x , float y) { 
  glm::ivec2 top_left = (glm::ivec2(x, y) / TILE_SIZE) * TILE_SIZE;
  if (x < 0 && fabs(top_left.x - x) > 0.00001) top_left.x -= TILE_SIZE;
  if (y < 0 && fabs(top_left.y - y) > 0.00001) top_left.y -= TILE_SIZE;

  float v[4];
  v[0] = clipmaps_[0].GetGridHeight(top_left.x                  , top_left.y                  );
  v[1] = clipmaps_[0].GetGridHeight(top_left.x                  , top_left.y + TILE_SIZE + 0.1);
  v[2] = clipmaps_[0].GetGridHeight(top_left.x + TILE_SIZE + 0.1, top_left.y + TILE_SIZE + 0.1);
  v[3] = clipmaps_[0].GetGridHeight(top_left.x + TILE_SIZE + 0.1, top_left.y                  );

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
