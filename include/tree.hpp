#ifndef _TREE_HPP_
#define _TREE_HPP_

#include <fstream>
#include <vector>
#include <iostream>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include "geometry.hpp"
#include "entity.hpp"
#include "config.h"

namespace Sibyl {

struct Branch {
  glm::vec3 head;
  glm::vec3 tangent;
  float length;
  int trunk_type;
  int *ring0, *ring1, *ring2;
  int *root_ring;
  float radius;
  int end;

  Branch* parent;
  Branch* child0;
  Branch* child1;

  Branch();
  Branch(glm::vec3 aHead, Branch *aParent);
  ~Branch();
};

class Tree : public IEntity {
  glm::vec3 position_;
  Shader shader_;
  Shader twig_shader_;
  GLuint vertex_buffer_;
  GLuint uv_buffer_;
  GLuint normal_buffer_;
  GLuint element_buffer_;
  GLuint twig_vertex_buffer_;
  GLuint twig_uv_buffer_;
  GLuint twig_normal_buffer_;
  GLuint twig_element_buffer_;
  unsigned int sizes_;

  std::vector<glm::vec3> vertices_;
  std::vector<glm::vec2> uvs_;
  std::vector<glm::vec3> normals_;
  std::vector<unsigned int> indices_;

  std::vector<glm::vec3> twig_vertices_;
  std::vector<glm::vec2> twig_uvs_;
  std::vector<glm::vec3> twig_normals_;
  std::vector<unsigned int> twig_indices_;
  std::vector<glm::ivec3> faces_;

  GLuint texture_;
  GLuint twig_texture_;
  GLuint twig_mask_texture_;

  // BEGIN - Properties.
  int seed_ = 262;
  int r_seed_ = 262;
  int segments_ = 12;
  int levels_ = 5;
  // float v_multiplier_ = 0.36f;
  float v_multiplier_ = 10.f;
  float twig_scale_ = 0.19f;
  float initial_branch_length_ = 0.49f;
  float length_falloff_factor_ = 0.85f;
  float length_falloff_power_ = 0.99f;
  float clump_max_ = 0.454f;
  float clump_min_ = 0.404f;
  float branch_factor_ = 2.45f;
  float drop_amount_ = -0.1f;
  float grow_amount_ = 0.235f;
  float sweep_amount_ = 0.01f;
  float max_radius_ = 0.139f;
  float climb_rate_ = 0.371f;
  float trunk_kink_ = 0.093f;
  int tree_steps_ = 5;
  float taper_rate_ = 0.947f;
  float radius_falloff_rate_ = 0.73f;
  float twist_rate_ = 1.02f;
  float trunk_length_ = 2.4f;

  Branch* root_;
  int vert_count_ = 0;
  int twig_vert_count_ = 0;
  int face_count_ = 0;
  int twig_face_count_ = 0;

  glm::vec3* vert_;
  glm::vec3* normal_;
  glm::vec2* uv_;
  glm::vec3* twig_vert_;
  glm::vec3* twig_normal_;
  glm::vec2* twig_uv_;
  glm::ivec3* face_;
  glm::ivec3* twig_face_;
  // END - Properties.

  void Generate();
  glm::vec3 MirrorBranch(glm::vec3, glm::vec3);
  float GetRandom(float);
  void Split(Branch* branch, int aLevel, int aSteps, int aL1 = 1, int aL2 = 1);
  void CreateForks(Branch*, float);
  void CreateTwigs(Branch*);
  void DoFaces(Branch*);
  void PrintBranch(Branch*, int);

 public:
  Tree(Shader, Shader, GLuint, GLuint, GLuint);

  void Draw(glm::mat4, glm::mat4, glm::vec3);
  void Clean() {}
  std::vector<glm::vec3> vertices() { return std::vector<glm::vec3>(); }
  void set_position(glm::vec3 position) { position_ = position; }
};

} // End of namespace.

#endif
