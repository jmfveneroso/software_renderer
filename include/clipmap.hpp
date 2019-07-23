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
#include "subregion.hpp"
#include "config.h"

namespace Sibyl {

struct HeightBuffer {
  glm::ivec2 top_left = glm::ivec2(1, 1);

  float row_heights[CLIPMAP_SIZE + 1][CLIPMAP_SIZE + 1];
  float column_heights[CLIPMAP_SIZE + 1][CLIPMAP_SIZE + 1];
  glm::vec3 row_normals[CLIPMAP_SIZE + 1][CLIPMAP_SIZE + 1];
  glm::vec3 column_normals[CLIPMAP_SIZE + 1][CLIPMAP_SIZE + 1];

  float valid_rows[CLIPMAP_SIZE + 1];
  float valid_columns[CLIPMAP_SIZE + 1];
};

class Clipmap {
  float* height_map_;

  unsigned int level_;
  HeightBuffer height_buffer_;

  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint element_buffer_;

  GLuint height_texture_;
  GLuint normals_texture_;
  GLuint tangents_texture_;
  GLuint bitangents_texture_;

  glm::ivec2 top_left_;
  int num_invalid_ = (CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1);
  glm::vec3 vertices_[(CLIPMAP_SIZE+1) * (CLIPMAP_SIZE+1)];

  Subregion subregions_[5];

  int GetTileSize();
  glm::ivec2 WorldToGridCoordinates(glm::vec3);
  glm::vec3 GridToWorldCoordinates(glm::ivec2);
  glm::ivec2 ClampGridCoordinates(glm::ivec2, int);
  glm::ivec2 GridToBufferCoordinates(glm::ivec2);
  glm::ivec2 BufferToGridCoordinates(glm::ivec2);
  void InvalidateOuterBuffer(glm::ivec2);
  float GetGridHeight(float, float);

 public:
  Clipmap();
  Clipmap(float*, unsigned int);

  void Render(glm::vec3, Shader*, glm::mat4, glm::mat4, bool);
  void RenderWater(glm::vec3, Shader*, glm::mat4, glm::mat4, glm::vec3, bool);
  void Init();
  void Update(glm::vec3);
  void UpdatePoint(int, int, float*, glm::vec3*);
};

} // End of namespace.

#endif
