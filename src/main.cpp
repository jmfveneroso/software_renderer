#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <map>
#include <math.h>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include "shaders.h"
#include "bitmap.h"
#include "controls.h"
#include "tangentspace.h"
#include "objloader.h"
#include "vbo_indexer.h"
#include "render_object.h"
#include "water.h"
#include "renderer.h"

GLFWwindow* window;
int main() {
  if (!glfwInit()) {
    fprintf( stderr, "Failed to initialize GLFW\n" );
    getchar();
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  int windowWidth = 600;
  int windowHeight = 400;
  window = glfwCreateWindow(windowWidth, windowHeight, "Test", NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window.\n" );
    glfwTerminate();
    return -1;
  }

  // We would expect width and height to be 1024 and 768
  // But on MacOS X with a retina screen it'll be 1024*2 and 768*2, so we get the actual framebuffer size:
  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
  glfwMakeContextCurrent(window);

  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  // Hide the mouse and enable unlimited movement
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwPollEvents();
  glfwSetCursorPos(window, windowWidth/2, windowHeight/2);
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); 
  glEnable(GL_CULL_FACE);
  glEnable(GL_CLIP_PLANE0);

  // Why is this necessary? Should look on shaders.
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  Renderer renderer(windowWidth, windowHeight);
  renderer.CreateScene();

  double lastTime = glfwGetTime();
  int nbFrames = 0;
  do {
    // Measure speed
    double currentTime = glfwGetTime();
    nbFrames++;
    if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
      // printf and reset
      printf("%f ms/frame\n", 1000.0/double(nbFrames));
      nbFrames = 0;
      lastTime += 1.0;
    }

    renderer.DrawScene();

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  } // Check if the ESC key was pressed or the window was closed
  while (
    glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0 
  );

  // Cleanup VBO and shader
  renderer.Clean();

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
