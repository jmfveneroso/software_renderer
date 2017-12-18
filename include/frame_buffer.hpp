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
  GLuint FramebufferName;
  GLuint depthrenderbuffer;
  GLuint renderedTexture;
  GLuint depthTexture;
  GLuint texID;
  GLuint quad_programID;
  GLuint quad_vertexbuffer;
  GLuint topleftID;
  GLuint DepthMapID_;
  glm::vec2 topleft;
  int windowWidth, windowHeight;

 public:
  FrameBuffer() {}
  FrameBuffer(int windowWidth, int windowHeight, glm::vec2 topleft);

  void Initialize();
  void Draw();
  GLuint GetTexture() { return renderedTexture; }
  GLuint GetDepthTexture() { return depthTexture; }
  GLuint GetFramebuffer() { return FramebufferName; }
};

} // End of namespace.

#endif
