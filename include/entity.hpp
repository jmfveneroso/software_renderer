#ifndef _ENTITY_HPP_
#define _ENTITY_HPP_

#include <vector>
#include <fstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

using namespace glm;
#include "controls.h"
#include "shaders.h"
#include "bitmap.h"
#include "tangentspace.h"
#include "objloader.h"
#include "vbo_indexer.h"
#include "config.h"

extern bool over_ground;
extern glm::vec3 fall_speed;

namespace Sibyl {

class IEntity {
 public:
  ~IEntity() {}

  virtual void Draw(glm::mat4, glm::mat4, glm::vec3) = 0;
  virtual std::vector<glm::vec3> vertices() = 0;
  virtual void set_position(glm::vec3) = 0;
  virtual void Clean() = 0;
};

class Solid : public IEntity {
 protected:
  std::vector<glm::vec3> vertices_;
  std::vector<unsigned short> indices_;
  glm::vec3 position_;
  Shader shader_;
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint normal_buffer_;
  GLuint tangent_buffer_;
  GLuint bitangent_buffer_;
  GLuint element_buffer_;
  GLuint diffuse_texture_id_;
  GLuint normal_texture_id_;
  GLuint specular_texture_id_;

 public:
  Solid(
    const std::string& filename, 
    Shader shader, 
    GLuint diffuse_texture_id, 
    GLuint normal_texture_id, 
    GLuint specular_texture_id
  );

  void LoadModel(const std::string& filename);
  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void Clean();

  std::vector<glm::vec3> vertices() { return vertices_; }
  void set_position(glm::vec3 position) { position_ = position; }
};

class Water : public Solid {
  GLuint reflection_texture_id_;
  GLuint refraction_texture_id_;
  GLuint refraction_depth_texture_id_;

 public:
  static float move_factor;

  Water(
    const std::string& filename, 
    Shader shader, 
    GLuint diffuse_texture_id, 
    GLuint normal_texture_id, 
    GLuint reflection_texture_id,
    GLuint refraction_texture_id,
    GLuint refraction_depth_texture_id
  );

  static void UpdateMoveFactor(float);
  void Draw(glm::mat4, glm::mat4, glm::vec3);
};

} // End of namespace.

#endif
