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

#define TILE_SIZE 100
#define QUAD_SIZE 128
#define BIG_QUAD_SIDE 20
#define NUM_QUADS (BIG_QUAD_SIDE * BIG_QUAD_SIDE)

#define BLA QUAD_SIZE * QUAD_SIZE
#define BLI BLA * 4
#define BLE BLA * 6

namespace Sibyl {

struct TerrainQuad {
  int x, y;
  int level_of_detail;

  bool initialized;
  bool empty;
  int distance;

  unsigned int indices[BLE];
  glm::vec3 indexed_vertices  [BLI];
  glm::vec2 indexed_uvs       [BLI];
  glm::vec3 indexed_normals   [BLI];
  glm::vec3 indexed_tangents  [BLI];
  glm::vec3 indexed_bitangents[BLI];

  int buffer_size;
  int lod_size;
  unsigned int actual_indices[BLE];

  GLuint vertex_buffer;
  GLuint uv_buffer;
  GLuint normal_buffer;
  GLuint tangent_buffer;
  GLuint bitangent_buffer;
  GLuint element_buffer;

  TerrainQuad() : x(0), y(0), empty(true), initialized(false) {
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &uv_buffer);
    glGenBuffers(1, &normal_buffer);
    glGenBuffers(1, &tangent_buffer);
    glGenBuffers(1, &bitangent_buffer);
    glGenBuffers(1, &element_buffer);
  }
};

class Terrain : public IEntity {
  int last_center_x_;
  int last_center_y_;

  int quad_indices_[BIG_QUAD_SIDE][BIG_QUAD_SIDE];
  TerrainQuad quads_[NUM_QUADS];

  glm::vec3 prev_player_position_;
  std::shared_ptr<Player> player_;
  Shader shader_;
  glm::vec3 position_;
  GLuint diffuse_texture_id_;
  GLuint normal_texture_id_;
  GLuint specular_texture_id_;

  std::vector<unsigned int> indices_;
  std::vector<glm::vec3> indexed_vertices_;
  std::vector<glm::vec2> indexed_uvs_;
  std::vector<glm::vec3> indexed_normals_;
  std::vector<glm::vec3> indexed_tangents_;
  std::vector<glm::vec3> indexed_bitangents_;

  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint normal_buffer_;
  GLuint tangent_buffer_;
  GLuint bitangent_buffer_;
  GLuint element_buffer_;

  SimplexNoise noise_;

  unsigned int AddVertexToQuad(TerrainQuad*, int, glm::vec3, float, float, glm::vec3 normal = glm::vec3(0,0,0));
  void UpdateQuad(int, int, int, int);

 public:
  Terrain(
    std::shared_ptr<Player> player,
    Shader shader, 
    GLuint diffuse_texture_id, 
    GLuint normal_texture_id, 
    GLuint specular_texture_id
  );

  void UpdateQuads();
  float GetHeight(float, float);
  std::vector<glm::vec3> vertices() { return indexed_vertices_; }
  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void set_position(glm::vec3 v) {}
  void Clean();
};

} // End of namespace.

#endif
