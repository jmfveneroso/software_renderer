#ifndef _ENTITY_HPP_
#define _ENTITY_HPP_

#include <vector>
#include <fstream>
#include <cstring>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

#include "shaders.h"
#include "config.h"

extern bool over_ground;
extern glm::vec3 fall_speed;

namespace Sibyl {

class IEntity {
 protected:
  bool LoadObj(
    const char*, 
    std::vector<glm::vec3>&, 
    std::vector<glm::vec2>&,
    std::vector<glm::vec3>&
  );

  void ComputeTangentBasis(
    std::vector<glm::vec3>&,
    std::vector<glm::vec2>&,
    std::vector<glm::vec3>&,
    std::vector<glm::vec3>&,
    std::vector<glm::vec3>&
  );

  bool IsNear(float, float);

  bool GetSimilarVertexIndex( 
    glm::vec3&, 
    glm::vec2&, 
    glm::vec3&, 
    std::vector<glm::vec3>&,
    std::vector<glm::vec2>&,
    std::vector<glm::vec3>&,
    unsigned short&
  );

  void IndexVBO(
    std::vector<glm::vec3>&,
    std::vector<glm::vec2>&,
    std::vector<glm::vec3>&,
    std::vector<glm::vec3>&,
    std::vector<glm::vec3>&,
    std::vector<unsigned short>&,
    std::vector<glm::vec3>&,
    std::vector<glm::vec2>&,
    std::vector<glm::vec3>&,
    std::vector<glm::vec3>&,
    std::vector<glm::vec3>&
  );

  void ComputeIndexedVertices(
    std::vector<glm::vec3>&,
    std::vector<glm::vec2>&,
    std::vector<glm::vec3>&,
    std::vector<unsigned short>& indices,
    GLuint *vertex_buffer,
    GLuint *uv_buffer,
    GLuint *normal_buffer,
    GLuint *tangent_buffer,
    GLuint *bitangent_buffer,
    GLuint *element_buffer
  );

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

class Terrain : public IEntity {
  Shader shader_;
  std::vector<unsigned short> indices_;
  std::vector<glm::vec3> vertices_;
  std::vector<glm::vec2> uvs_;
  std::vector<glm::vec3> normals_;
  GLuint diffuse_texture_id_;
  GLuint normal_texture_id_;
  GLuint specular_texture_id_;

 public:
  Terrain(
    Shader shader, 
    GLuint diffuse_texture_id, 
    GLuint normal_texture_id, 
    GLuint specular_texture_id
  );

  std::vector<glm::vec3> vertices() { return vertices_; }
  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void set_position(glm::vec3 v) {}
  void Clean() {}
};

} // End of namespace.

#endif
