#ifndef _RENDER_OBJECT_H_
#define _RENDER_OBJECT_H_

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

struct AABB {
  glm::vec3 min, max;
};

extern bool over_ground;
extern glm::vec3 fall_speed;

namespace Sibyl {

class IEntity {
 public:
  ~IEntity() {}

  virtual void Draw(glm::mat4, glm::mat4, glm::vec3) = 0;
  virtual std::vector<glm::vec3> vertices() = 0;
  virtual void set_position(glm::vec3) = 0;
};

class Solid : public IEntity {
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
  ) : shader_(shader),
      diffuse_texture_id_(diffuse_texture_id), 
      normal_texture_id_(normal_texture_id), 
      specular_texture_id_(specular_texture_id),
      position_(glm::vec3(0.0, 0.0, 0.0)) {
    LoadModel(filename);
  }

  void LoadModel(const std::string& filename) {
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals; 
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    ::loadOBJ(filename.c_str(), vertices_, uvs, normals);
    ::computeTangentBasis(
      vertices_, uvs, normals, // Input.
      tangents, bitangents    // Output.
    );

    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    std::vector<glm::vec3> indexed_tangents;
    std::vector<glm::vec3> indexed_bitangents;
    ::indexVBO_TBN(
      vertices_, uvs, normals, tangents, bitangents, 
      indices_, indexed_vertices, indexed_uvs, 
      indexed_normals, indexed_tangents, indexed_bitangents
    );
    
    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &uv_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &normal_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &tangent_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, tangent_buffer_);
    glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &bitangent_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, bitangent_buffer_);
    glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &element_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned short), &indices_[0] , GL_STATIC_DRAW);
  }

  void Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
    glUseProgram(shader_.program_id());

    // Uniforms.
    glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
    glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
    glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(shader_.GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix3fv(shader_.GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

    glm::vec3 lightPos = glm::vec3(0, 2000, 0);
    glUniform3f(shader_.GetUniformId("LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);

    glm::vec4 plane = vec4(0, -1, 0, 10000);
    glUniform4fv(shader_.GetUniformId("plane"), 1, (float*) &plane);
    glUniform1i(shader_.GetUniformId("use_normals"), false);
    glUniform1i(shader_.GetUniformId("water_fog"), false);

    // Textures.
    shader_.BindTexture("DiffuseTextureSampler", diffuse_texture_id_);
    shader_.BindTexture("NormalTextureSampler", normal_texture_id_);
    shader_.BindTexture("SpecularTextureSampler", specular_texture_id_);

    // Buffers.
    shader_.BindBuffer(vertex_buffer_, 0, 3);
    shader_.BindBuffer(uv_buffer_, 1, 2);
    shader_.BindBuffer(normal_buffer_, 2, 3);
    shader_.BindBuffer(tangent_buffer_, 3, 3);
    shader_.BindBuffer(bitangent_buffer_, 4, 3);
    
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, (void*) 0);

    shader_.Clear();
  }

  std::vector<glm::vec3> vertices() { return vertices_; };
  void set_position(glm::vec3 position) { position_ = position; };
};

} // End of namespace.

#endif
