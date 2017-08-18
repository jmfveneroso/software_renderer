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

#include "bsp_tree.h"
#include "render.h"
#include "visual_engine.h"

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path){
  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
  if(VertexShaderStream.is_open()){
    std::string Line = "";
    while(getline(VertexShaderStream, Line))
      VertexShaderCode += "\n" + Line;
    VertexShaderStream.close();
  }else{
    printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
    getchar();
    return 0;
  }

  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
  if(FragmentShaderStream.is_open()){
    std::string Line = "";
    while(getline(FragmentShaderStream, Line))
      FragmentShaderCode += "\n" + Line;
    FragmentShaderStream.close();
  }

  GLint Result = GL_FALSE;
  int InfoLogLength;

  // Compile Vertex Shader
  printf("Compiling shader : %s\n", vertex_file_path);
  char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    printf("%s\n", &VertexShaderErrorMessage[0]);
  }

  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragment_file_path);
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    printf("%s\n", &FragmentShaderErrorMessage[0]);
  }

  // Link the program
  printf("Linking program\n");
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> ProgramErrorMessage(InfoLogLength+1);
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
  }

  
  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);
  
  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}

GLuint CreateTexture(int x) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // bool even = true;
  // int counter = 0;
  // for (int i = 0; i < windowHeight; ++i) {
  //   for (int j = 0; j < windowWidth; ++j) {
  //     if (i % x < x/2) {
  //       data[i * windowWidth * 3 + j * 3]     = 0;
  //       data[i * windowWidth * 3 + j * 3 + 1] = 0;
  //       data[i * windowWidth * 3 + j * 3 + 2] = 255;
  //     } else {
  //       data[i * windowWidth * 3 + j * 3]     = 255;
  //       data[i * windowWidth * 3 + j * 3 + 1] = 255;
  //       data[i * windowWidth * 3 + j * 3 + 2] = 255;
  //     }
  //     even = !even;
  //   }
  // }

  // for (int j = 0; j < windowWidth; ++j) {
  //   DrawPixel(j, 100, 0, 0, 255);
  // }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  return texture;
}

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

  int windowWidth = 1000;
  int windowHeight = 600;
  data = new unsigned char[windowWidth * windowHeight * 3];
  window = glfwCreateWindow(windowWidth, windowHeight, "Test", NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
    getchar();
    glfwTerminate();
    return -1;
  }

  // We would expect width and height to be 1024 and 768
  // But on MacOS X with a retina screen it'll be 1024*2 and 768*2, so we get the actual framebuffer size:
  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE; 
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  // Hide the mouse and enable unlimited mouvement
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  
  // Create and compile our GLSL program from the shaders
  GLuint programID = LoadShaders("vshade", "fshade");
  GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

  // The fullscreen quad's FBO
  GLuint quad_VertexArrayID;
  glGenVertexArrays(1, &quad_VertexArrayID);
  glBindVertexArray(quad_VertexArrayID);
  
  static const GLfloat g_quad_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
  };

  GLuint quad_vertexbuffer;
  glGenBuffers(1, &quad_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

  double lastTime = glfwGetTime();
  int nbFrames = 0;

  GLuint texture;
  int counter = 2;




  double speed = 5;
  player.pos.x = 10;
  player.pos.y = 10;
  player.angle = 0;
  player.v_angle = 0;
  player.height = 400;

  pfrustum.distance = 40;
  pfrustum.size = 60;
  
  InitVisualEngine();
  // CreateWalls();
  // InitRenderer();

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

    // Calculate motion.

    if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
      player.pos.x += speed * cos(player.angle);
      player.pos.y += speed * sin(player.angle);
    }

    if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
      player.pos.x -= speed * cos(player.angle);
      player.pos.y -= speed * sin(player.angle);
    }

    if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
      player.angle += 0.03;
    }

    if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
      player.angle -= 0.03;
    }

    if (glfwGetKey( window, GLFW_KEY_Q ) == GLFW_PRESS){
      player.height -= 20;
    }

    if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
      player.height += 20;
    }

    if (glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS){
      player.v_angle -= 3;
      // if (player.v_angle < -M_PI + 0.1) player.v_angle = -M_PI + 0.1;
    }

    if (glfwGetKey( window, GLFW_KEY_R ) == GLFW_PRESS){
      player.v_angle += 3;
      // if (player.v_angle > M_PI - 0.1) player.v_angle = M_PI - 0.1;
    }

    if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS)
      sectors[4]->height += 10;
    if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS)
      sectors[4]->height -= 10;

    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID);

    glActiveTexture(GL_TEXTURE0);

    Render();
    texture = CreateTexture(counter++); 

    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(TextureID, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glVertexAttribPointer(
    	0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    	3,                  // size
    	GL_FLOAT,           // type
    	GL_FALSE,           // normalized?
    	0,                  // stride
    	(void*)0            // array buffer offset
    );

    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
    glDisableVertexAttribArray(0);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
    glDeleteTextures(1, &texture);
  
  } // Check if the ESC key was pressed or the window was closed
  while (
    glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0 
  );

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
