#ifndef _TERRAIN_HPP_
#define _TERRAIN_HPP_

#include <algorithm>
#include <vector>
#include <fstream>
#include <cstring>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include "shaders.h"
#include "mesh.hpp"
#include "player.hpp"
#include "entity.hpp"
#include "simplex_noise.hpp"
#include "config.h"

#define CLIPMAP_LEVELS 11
#define CLIPMAP_SIZE 90
#define TILE_SIZE 64
#define CLIPMAP_OFFSET ((CLIPMAP_SIZE - 2) / 2)

namespace Sibyl {

enum RenderRegion {
  RR_LEFT,
  RR_TOP,
  RR_BOTTOM,
  RR_RIGHT
};

struct HeightBuffer {
  glm::ivec2 top_left;

  float height[(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];
  float valid[(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];
  glm::vec3 normals[(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];

  glm::ivec2 world_coords[(CLIPMAP_SIZE+1)];
};

class Clipmap {
  unsigned int level_;
  SimplexNoise noise_;
  HeightBuffer height_buffer_;

  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint barycentric_buffer_;
  GLuint element_buffer_;

  GLuint subregion_buffers_[5];
  unsigned int subregion_indices_[5][(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];
  unsigned int subregion_sizes_[5];

  GLuint height_texture_;
  GLuint normals_texture_;
  GLuint valid_texture_;


  std::vector<unsigned int> indices_;
  glm::vec3 vertices_[(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];

  glm::ivec2 top_left_;


  glm::ivec2 WorldToGridCoordinates(glm::vec3);
  glm::vec3 GridToWorldCoordinates(glm::ivec2);
  glm::ivec2 ClampGridCoordinates(glm::ivec2, int);
  glm::ivec2 GridToBufferCoordinates(glm::ivec2);
  glm::ivec2 BufferToGridCoordinates(glm::ivec2);
  void InvalidateOuterBuffer(glm::ivec2);
  void UpdateHeightMap();

  GLuint center_region_buffer_;
  int center_region_size_;
  GLuint render_region_buffers_[2][2][4];
  int render_region_sizes_[2][2][4];
  int CreateRenderRegion(glm::ivec2, glm::ivec2);

 public:
  Clipmap();
  Clipmap(unsigned int);

  int GetTileSize();
  void Render(glm::vec3, Shader*, glm::mat4, glm::mat4);
  float GetHeight(float, float);
  void Init();
  void Update(glm::vec3);

  GLuint vertex_buffer() { return vertex_buffer_; }
  GLuint uv_buffer() { return uv_buffer_; }
  GLuint element_buffer() { return element_buffer_; }
  std::vector<unsigned int> indices() { return indices_; }
  GLuint barycentric_buffer() { return barycentric_buffer_; }
  void set_level(unsigned int level) { level_ = level; }
  glm::ivec2 top_left() { return top_left_; }
  void set_top_left(glm::ivec2 top_left) { top_left_ = top_left; }
};

class Terrain : public IEntity {
  Clipmap clipmaps_[CLIPMAP_LEVELS]; 

  std::shared_ptr<Player> player_;
  SimplexNoise noise_;
  Shader shader_;
  GLuint diffuse_texture_id_;
  GLuint normal_texture_id_;
  GLuint specular_texture_id_;

 public:
  Terrain(
    std::shared_ptr<Player> player,
    Shader shader, 
    GLuint diffuse_texture_id, 
    GLuint normal_texture_id, 
    GLuint specular_texture_id
  );

  float GetHeight(float x , float y) { return clipmaps_[0].GetHeight(x, y); }
  void Draw(glm::mat4, glm::mat4, glm::vec3);

  std::vector<glm::vec3> vertices() { return std::vector<glm::vec3>(); }
  void set_position(glm::vec3 v) {}
  void Clean() {}
};

} // End of namespace.

#endif
