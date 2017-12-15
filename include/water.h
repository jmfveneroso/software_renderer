#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <iostream>
#include <glm/gtx/norm.hpp>
#include "shaders.h"

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

  FrameBuffer(int windowWidth, int windowHeight, vec2 topleft) 
    : windowWidth(windowWidth), 
      windowHeight(windowHeight), 
      topleft(topleft) {
    Initialize();
  }

  void Initialize() {
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    
    // The texture we're going to render to.
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    
    // Poor filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
     
    // The depth buffer.
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
    
    // Alternative: Depth texture. Slower, but you can sample it later in your shader.
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 1024, 768, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    // Set "renderedTexture" as our colour attachement #0.
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
    
    // Depth texture alternative.
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
    
    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers.
    
    // Always check that our framebuffer is ok.
    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //   return false;

    // The fullscreen quad's FBO.
    static const GLfloat g_quad_vertex_buffer_data[] = { 
      -1.0f, -1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
      -1.0f,  1.0f, 0.0f,
      -1.0f,  1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
       1.0f,  1.0f, 0.0f
    };

    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    
    // Create and compile our GLSL program from the shaders.
    quad_programID = LoadShaders("shaders/vshade_static_screen", "shaders/fshade_static_screen");
    texID = glGetUniformLocation(quad_programID, "renderedTexture");
    topleftID = glGetUniformLocation(quad_programID, "topleft");
    DepthMapID_ = glGetUniformLocation(quad_programID, "depthMap");
  }

  GLuint GetTexture() { return renderedTexture; }
  GLuint GetDepthTexture() { return depthTexture; }
  GLuint GetFramebuffer() { return FramebufferName; }

  void Draw() {
    glUseProgram(quad_programID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glUniform1i(texID, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(DepthMapID_, 1);

    glUniform2fv(topleftID, 1, (float*) &topleft);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles.
    glDisableVertexAttribArray(0);
  }
};

#endif
