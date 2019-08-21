#ifndef _TERRAIN_HPP_
#define _TERRAIN_HPP_

#include <algorithm>
#include <vector>
#include <iostream>
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
#include "config.h"
#include "clipmap.hpp"

namespace Sibyl {

class Terrain {
  Clipmap clipmaps_[CLIPMAP_LEVELS]; 
  // float* height_map_;
  vector< vector<float> > height_map_;

  Shader shader_;
  Shader water_shader_;
  GLuint grass_texture_id_;
  GLuint sand_texture_id_;
  GLuint water_diffuse_texture_id_;
  GLuint water_normal_texture_id_;

 public:
  Terrain(
    Shader shader, 
    Shader water_shader, 
    GLuint grass_texture_id, 
    GLuint sand_texture_id,
    GLuint water_diffuse_texture_id,
    GLuint water_normal_texture_id
  );

  void LoadTerrain(const string& filename);
  float GetHeight(float x , float y);
  void Draw(glm::mat4, glm::mat4, glm::vec3, glm::vec3);
};

} // End of namespace.

#endif
