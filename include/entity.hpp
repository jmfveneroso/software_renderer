#ifndef _ENTITY_HPP_
#define _ENTITY_HPP_

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
#include "simplex_noise.hpp"
#include "config.h"

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
  Mesh mesh_;
  glm::vec3 position_;
  Shader shader_;

  void BindBuffers();

 public:
  GLuint diffuse_texture_id_;
  GLuint normal_texture_id_;
  GLuint specular_texture_id_;

  Solid(
    const std::string& filename, 
    Shader shader, 
    GLuint diffuse_texture_id, 
    GLuint normal_texture_id, 
    GLuint specular_texture_id
  );

  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void Clean();

  std::vector<glm::vec3> vertices() { return mesh_.vertices(); }
  void set_position(glm::vec3 position) { position_ = position; }
};

class Water : public Solid {
 public:
  GLuint reflection_texture_id_;
  GLuint refraction_texture_id_;
  GLuint refraction_depth_texture_id_;

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

class TextureScreen : public IEntity {
 protected:
  glm::vec3 position_;
  Shader shader_;
  GLuint texture_id_;

 public:
  TextureScreen(
    Shader shader,
    GLuint texture_id
  );

  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void Clean();

  std::vector<glm::vec3> vertices() { return std::vector<glm::vec3>(); }
  void set_position(glm::vec3 position) { position_ = position; }
};

} // End of namespace.

#endif
