#ifndef _MESH_HPP_
#define _MESH_HPP_

#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include "config.h"

namespace Sibyl {

struct Vertex {
  glm::vec3 pos;
  glm::vec2 uv;
  glm::vec3 normal;

  Vertex(
    glm::vec3 pos,
    glm::vec2 uv,
    glm::vec3 normal
  ) : 
    pos(pos),
    uv(uv),
    normal(normal) {
  }
};

class Mesh {
  std::vector<glm::vec3> vertices_;
  std::vector<glm::vec2> uvs_;
  std::vector<glm::vec3> normals_;
  std::vector<glm::vec3> tangents_;
  std::vector<glm::vec3> bitangents_;
  std::vector<glm::vec3> indexed_vertices_;
  std::vector<glm::vec2> indexed_uvs_;
  std::vector<glm::vec3> indexed_normals_;
  std::vector<glm::vec3> indexed_tangents_;
  std::vector<glm::vec3> indexed_bitangents_;
  std::vector<unsigned short> indices_;

  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint normal_buffer_;
  GLuint tangent_buffer_;
  GLuint bitangent_buffer_;
  GLuint element_buffer_;

  void ComputeTangentBasis();
  bool IsNear(float, float);
  bool GetSimilarVertexIndex(unsigned short, unsigned short&);
  void IndexVBO();

 public:
  Mesh() {}

  void CreateBuffers();
  bool LoadObj(const std::string&);
  void Clean();
  void AddVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal) { 
    vertices_.push_back(pos);
    uvs_.push_back(uv);
    normals_.push_back(normal);
  }
  
  GLuint vertex_buffer()    { return vertex_buffer_;    }
  GLuint uv_buffer()        { return uv_buffer_;        }
  GLuint normal_buffer()    { return normal_buffer_;    }
  GLuint tangent_buffer()   { return tangent_buffer_;   }
  GLuint bitangent_buffer() { return bitangent_buffer_; }
  GLuint element_buffer()   { return element_buffer_;   }
  std::vector<glm::vec3> indexed_vertices()   { return indexed_vertices_;   }
  std::vector<glm::vec2> indexed_uvs()        { return indexed_uvs_;        }
  std::vector<glm::vec3> indexed_normals()    { return indexed_normals_;    }
  std::vector<glm::vec3> indexed_tangents()   { return indexed_tangents_;   }
  std::vector<glm::vec3> indexed_bitangents() { return indexed_bitangents_; }

  std::vector<unsigned short> indices() { return indices_; }
  std::vector<glm::vec3> vertices() { return vertices_; }
};

} // End of namespace.

#endif
