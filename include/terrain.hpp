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
#include "rock.hpp"
#include "config.h"
#include "clipmap.hpp"

namespace Sibyl {

struct TerrainFeature {
  int id;
  glm::vec3 position;
  glm::vec3 color;
  glm::vec2 rotation;
  float scale;
  TerrainFeature(
    int id,
    glm::vec3 position,
    glm::vec3 color,
    glm::vec2 rotation,
    float scale
  ) : id(id),
      position(position),
      color(color),
      rotation(rotation),
      scale(scale) {
  }
};

class Terrain : public IEntity {
  Clipmap clipmaps_[CLIPMAP_LEVELS]; 

  std::shared_ptr<Rock> rocks_[10];
  std::shared_ptr<Player> player_;
  std::shared_ptr<HeightMap> height_map_;
  SimplexNoise noise_;
  Shader shader_;
  Shader rock_shader_;
  Shader water_shader_;
  GLuint diffuse_texture_id_;
  GLuint normal_texture_id_;
  GLuint specular_texture_id_;
  GLuint rock_texture_id_;
  GLuint rock_2_texture_id_;
  GLuint sand_texture_id_;
  std::shared_ptr<Water> water_;
  bool draw_water_ = false;
  glm::vec4 clip_plane_;
  std::vector<TerrainFeature> features_;

  void CreateTerrainFeatures();

 public:
  Terrain(
    std::shared_ptr<Player> player,
    Shader shader, 
    Shader rock_shader, 
    Shader water_shader, 
    GLuint diffuse_texture_id, 
    GLuint normal_texture_id, 
    GLuint specular_texture_id,
    GLuint rock_texture_id,
    GLuint rock_2_texture_id,
    GLuint sand_texture_id,
    std::shared_ptr<Water>
  );

  float GetHeight(float x , float y) { return height_map_->GetHeight(x, y); }
  void Draw(glm::mat4, glm::mat4, glm::vec3);

  std::vector<glm::vec3> vertices() { return std::vector<glm::vec3>(); }
  void set_position(glm::vec3 v) {}
  void Clean() {}
  void Erode();
  void DrawWater(bool flag) { draw_water_ = flag; }
  void SetClipPlane(glm::vec4 plane) { clip_plane_ = plane; }
  std::vector<TerrainFeature> features() { return features_; }
};

} // End of namespace.

#endif
