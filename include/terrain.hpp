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

#define CLIPMAP_LEVELS 1
#define CLIPMAP_SIZE 6
#define TILE_SIZE 32

namespace Sibyl {

class Clipmap {
  unsigned int level_;
  GLuint vertex_buffer_;
  GLuint height_buffer_;

  GLuint uv_buffer_;
  GLuint barycentric_buffer_;
  GLuint element_buffer_;
  std::vector<unsigned int> indices_;
  bool valid_[(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];

  glm::vec3 vertices_[(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];
  float height_[(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];

  GLuint height_texture_;
  SimplexNoise noise_;

  glm::ivec3 buffer_top_left_;
  glm::ivec3 top_left_;
  glm::ivec3 bottom_right_;
  float height_map_[(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];

  void DrawSubRegion(int, int, int, int);

  glm::ivec3 GetToroidalCoordinates(glm::ivec3);
  void InvalidateAll();
  void InvalidateColumns(glm::ivec3);
  void InvalidateRows(glm::ivec3);
  void Invalidate(glm::ivec3);

 public:
  Clipmap();
  Clipmap(unsigned int);

  unsigned int GetTileSize();
  void Render(glm::vec3, Shader*, glm::mat4, glm::mat4, glm::ivec3, glm::ivec3);
  float GetHeight(float, float);
  void Init();
  void Update(int, int);

  GLuint vertex_buffer() { return vertex_buffer_; }
  GLuint uv_buffer() { return uv_buffer_; }
  GLuint element_buffer() { return element_buffer_; }
  std::vector<unsigned int> indices() { return indices_; }
  GLuint barycentric_buffer() { return barycentric_buffer_; }
  void set_level(unsigned int level) { level_ = level; }
  glm::ivec3 top_left() { return top_left_; }
  glm::ivec3 bottom_right() { return bottom_right_; }
  void set_top_left(glm::ivec3 top_left) { top_left_ = top_left; }
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

  void Draw(glm::mat4, glm::mat4, glm::vec3);

  std::vector<glm::vec3> vertices() { return std::vector<glm::vec3>(); }
  void set_position(glm::vec3 v) {}
  void Clean() {}
};

} // End of namespace.

#endif
