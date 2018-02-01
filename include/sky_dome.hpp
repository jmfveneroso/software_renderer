#ifndef _SKY_DOME_HPP_
#define _SKY_DOME_HPP_

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
#include "entity.hpp"
#include "config.h"
#include "clipmap.hpp"

#define NUM_CIRCLES 8
#define NUM_POINTS_IN_CIRCLE 64
#define DOME_RADIUS 500000

namespace Sibyl {

class SkyDome : public IEntity {
  std::shared_ptr<Player> player_;
  Shader shader_;

  glm::ivec2 top_left_;
  GLuint texture_;
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint element_buffer_;
  unsigned char data_[2048 * 2048 * 3];
  std::vector<glm::vec3> vertices_;
  std::vector<glm::vec2> uvs_;
  std::vector<unsigned int> indices_;

  void CreateMesh();

 public:
  SkyDome(
    std::shared_ptr<Player> player,
    Shader shader
  );

  void Draw(glm::mat4, glm::mat4, glm::vec3);
  std::vector<glm::vec3> vertices() { return std::vector<glm::vec3>(); }
  void set_position(glm::vec3 v) {}
  void Clean() {}
};

} // End of namespace.

#endif
