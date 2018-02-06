#ifndef _FRAME_BUFFER_HPP_
#define _FRAME_BUFFER_HPP_

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <exception>
#include <memory>
#include "shaders.h"

namespace Sibyl {

class FrameBuffer {
  GLuint framebuffer_id_;
  GLuint depth_buffer_;
  GLuint rendered_texture_;
  GLuint depth_texture_;
  GLuint vertex_buffer_;
  int width_, height_;
  Shader shader_;

 public:
  FrameBuffer(Shader, int, int);

  void Init();
  void Draw();
  GLuint GetTexture() { return rendered_texture_; }
  GLuint GetDepthTexture() { return depth_texture_; }
  GLuint GetFramebuffer() { return framebuffer_id_; }
};

} // End of namespace.

#endif
