#ifndef _TERRAIN_HPP_
#define _TERRAIN_HPP_

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

namespace Sibyl {

class Terrain : public IEntity {
  Clipmap clipmaps_[CLIPMAP_LEVELS]; 

  std::shared_ptr<Player> player_;
  std::shared_ptr<HeightMap> height_map_;
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

  float GetHeight(float x , float y) { return height_map_->GetHeight(x, y); }
  void Draw(glm::mat4, glm::mat4, glm::vec3);

  std::vector<glm::vec3> vertices() { return std::vector<glm::vec3>(); }
  void set_position(glm::vec3 v) {}
  void Clean() {}
  void Erode();
};

} // End of namespace.

#endif
