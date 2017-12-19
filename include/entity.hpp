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

  GLuint diffuse_texture_id_;
  GLuint normal_texture_id_;
  GLuint specular_texture_id_;

  void BindBuffers();

 public:
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

class Terrain : public IEntity {
  Mesh mesh_;
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

  void CreateTiles();
  unsigned int AddVertex(float, float, float, float);

 public:
  Terrain(
    Shader shader, 
    GLuint diffuse_texture_id, 
    GLuint normal_texture_id, 
    GLuint specular_texture_id
  );

  std::vector<glm::vec3> vertices() { return mesh_.vertices(); }
  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void set_position(glm::vec3 v) {}
  void Clean();
};

} // End of namespace.

#endif
