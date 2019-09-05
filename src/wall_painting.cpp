#include "wall_painting.hpp"

using namespace std;
using namespace glm;

namespace Sibyl {

WallPainting::WallPainting(
  glm::vec3 position
) : shader_("painting", "v_painting", "f_painting"), 
    shader2_("lines", "v_lines", "f_lines"), 
    position_(position) {
  Init();
}

void WallPainting::Init() {
  // texture_ = Texture("textures/dirt.bmp");

  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &element_buffer_);

  float w = size_;
  float h = size_;
  float t = 0.1;

  vector<vec3> v {
    // Back face.
    vec3(0, 0, 0), vec3(w, 0, 0), vec3(0, -h, 0), vec3(w, -h, 0),
    // Front face.
    vec3(0, 0, t), vec3(w, 0, t), vec3(0, -h, t), vec3(w, -h, t),
  };

  vertices_ = {
    v[0], v[4], v[1], v[1], v[4], v[5], // Top.
    v[1], v[3], v[0], v[0], v[3], v[2], // Back.
    v[0], v[2], v[4], v[4], v[2], v[6], // Left.
    v[5], v[7], v[1], v[1], v[7], v[3], // Right.
    v[4], v[6], v[5], v[5], v[6], v[7], // Front.
    v[6], v[2], v[7], v[7], v[2], v[3]  // Bottom.
  };

  vector<vec2> u = {
    vec2(0, 0), vec2(0, 1), vec2(1, 0), vec2(1, 1)
  };

  vector<glm::vec2> uvs {
    u[0], u[0], u[0], u[0], u[0], u[0],   // Top.
    u[0], u[0], u[0], u[0], u[0], u[0],   // Back.
    u[0], u[0], u[0], u[0], u[0], u[0],   // Left.
    u[0], u[0], u[0], u[0], u[0], u[0],   // Right.
    u[0], u[1], u[2], u[2], u[1], u[3], // Front.
    u[0], u[0], u[0], u[0], u[0], u[0],   // Bottom.
  };

  for (int i = 0; i < 36; i++) indices_.push_back(i);

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_); 
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices_.size() * sizeof(unsigned int), 
    &indices_[0], 
    GL_STATIC_DRAW
  );
  
  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenFramebuffers(1, &frame_buffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_, 0);
  
  // Set the list of draw buffers.
  GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, DrawBuffers);
  
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    throw;

  DrawToTexture();
}

void WallPainting::DrawToTexture() {
  GLuint line_buffer;
  GLuint element_buffer;
  glGenBuffers(1, &line_buffer);
  glGenBuffers(1, &element_buffer);

  static std::vector<glm::vec3> lines = {
    vec3(0, 0, 0), vec3(10, 0, 0),
    vec3(0, 10, 0)
  };
  glBindBuffer(GL_ARRAY_BUFFER, line_buffer);
  glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(glm::vec3), &lines[0], GL_STATIC_DRAW);

  static std::vector<unsigned int> indices { 0, 1, 2 };
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices.size() * sizeof(unsigned int), 
    &indices[0], 
    GL_STATIC_DRAW
  );

  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
  glViewport(0, 0, 128, 128);
  glClearColor(0.7f, 0.7f, 0.7f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_CULL_FACE);

  glUseProgram(shader2_.program_id());

  glm::mat4 projection = glm::ortho(0.0f, 10.0f, 0.0f, 10.0f);
  glUniformMatrix4fv(shader2_.GetUniformId("projection"), 1, GL_FALSE, &projection[0][0]);

  shader2_.BindBuffer(line_buffer, 0, 3);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*) 0);

  shader2_.Clear();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glEnable(GL_CULL_FACE);
}

void WallPainting::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());

  shader_.BindTexture("TextureSampler", texture_);

  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat4 MVP = ProjectionMatrix * ModelViewMatrix;

  glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);

  shader_.BindBuffer(vertex_buffer_, 0, 3);
  shader_.BindBuffer(uv_buffer_, 1, 2);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*) 0);

  shader_.Clear();
}

} // End of namespace.
