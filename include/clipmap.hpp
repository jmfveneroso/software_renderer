#ifndef _CLIPMAP_HPP_
#define _CLIPMAP_HPP_

#include <algorithm>
#include <vector>
#include <memory>
#include <fstream>
#include <cstring>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include "shaders.h"
#include "mesh.hpp"
#include "player.hpp"
#include "height_map.hpp"
#include "entity.hpp"
#include "config.h"

#define CLIPMAP_LEVELS 11
#define CLIPMAP_SIZE 258
#define CLIPMAP_OFFSET ((CLIPMAP_SIZE - 2) / 2)

#define LEFT_BORDERS 14
#define TOP_BORDERS 4
#define BOTTOM_BORDERS 2
#define RIGHT_BORDERS 7
#define CENTER_BORDERS 15

namespace Sibyl {

enum RenderRegion {
  RR_LEFT = 0,
  RR_TOP,
  RR_BOTTOM,
  RR_RIGHT
};

struct HeightBuffer {
  glm::ivec2 top_left = glm::ivec2(1, 1);
  float height[(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];
  glm::vec3 normals[(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];
  float valid[(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];
};

class Clipmap {
  static std::vector<glm::ivec2> feature_points;
  std::shared_ptr<Player> player_;
  std::shared_ptr<HeightMap> height_map_;

  unsigned int level_;
  SimplexNoise noise_;
  HeightBuffer height_buffer_;

  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint barycentric_buffer_;
  GLuint element_buffer_;

  int active_texture_ = 0;
  GLuint height_texture_[2];
  GLuint normals_texture_[2];

  glm::ivec2 top_left_;
  int num_invalid_ = (CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1);
  glm::vec3 vertices_[(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];

  GLuint center_region_buffer_;
  int center_region_size_;
  GLuint render_region_buffers_[2][2][4];
  int render_region_sizes_[2][2][4];
  glm::ivec2 render_region_top_left_[2][2][4];
  glm::ivec2 render_region_clip_size_[2][2][4];

  int GetTileSize();
  glm::ivec2 WorldToGridCoordinates(glm::vec3);
  glm::vec3 GridToWorldCoordinates(glm::ivec2);
  glm::ivec2 ClampGridCoordinates(glm::ivec2, int);
  glm::ivec2 GridToBufferCoordinates(glm::ivec2);
  glm::ivec2 BufferToGridCoordinates(glm::ivec2);
  void InvalidateOuterBuffer(glm::ivec2);

  int CreateSubRegion(glm::ivec2, glm::ivec2, short);
  void CreateSubRegions();

 public:
  Clipmap();
  Clipmap(std::shared_ptr<Player>, std::shared_ptr<HeightMap>, unsigned int);

  void Render(glm::vec3, Shader*, glm::mat4, glm::mat4, bool);
  void RenderWater(glm::vec3, Shader*, glm::mat4, glm::mat4, glm::vec3, bool, std::shared_ptr<Water>);
  void Init();
  void Update(glm::vec3);
  void Clear();
};

} // End of namespace.

#endif
