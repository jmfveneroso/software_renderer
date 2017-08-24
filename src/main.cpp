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
#include "objloader.h"
#include "vbo_indexer.h"
#include "sphere.h"

GLuint CreateMvpMatrix(GLuint programID, glm::mat4* MVP) {
  GLuint MatrixID = glGetUniformLocation(programID, "MVP");

  // Projection matrix.
  glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

  glm::mat4 View = glm::lookAt(
    glm::vec3(4, 3, 3), // Camera position in World Space.
    glm::vec3(0, 0, 0), // Looking to location.
    glm::vec3(0, 1, 0)  // Head is up (set to 0, -1, 0 to look upside-down)
  );

  // Model matrix : an identity matrix (model will be at the origin)
  glm::mat4 Model = glm::mat4(1.0f);

  // Our ModelViewProjection : multiplication of our 3 matrices
  *MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
  return MatrixID;
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
  
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  // Create and compile our GLSL program from the shaders
  GLuint programID = LoadShaders("shaders/vshade", "shaders/fshade");
  
  glm::mat4 MVP;
  GLuint MatrixID = CreateMvpMatrix(programID, &MVP); 
  GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
  GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

  GLuint Texture = loadBMP_custom("textures/dirt.bmp");
  GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

  GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer;

  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals; // Won't be used at the moment.
  // bool res = loadOBJ("res/suzanne.obj", vertices, uvs, normals);
  bool res = loadOBJ("res/basic_terrain.obj", vertices, uvs, normals);

  std::vector<unsigned short> indices;
  std::vector<glm::vec3> indexed_vertices;
  std::vector<glm::vec2> indexed_uvs;
  std::vector<glm::vec3> indexed_normals;
  indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
  
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &uvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &elementbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

  glUseProgram(programID);
  GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

  double lastTime = glfwGetTime();
  int nbFrames = 0;

  Sphere::SetModel("res/icosphere.obj");
  Sphere spheres[2];
  spheres[0] = Sphere(-50.0f, 2.0f, -17.0f, 0.1f, 0.0f, 0.0f);
  spheres[1] = Sphere(50.0f, 2.0f, -17.0f, -0.1f, 0.0f,  0.0f);

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

    // Render to the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID);
    computeMatricesFromInputs();
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    
    glm::vec3 lightPos = glm::vec3(0,100,0);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(TextureID, 0);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
    	0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
    	3,                  // size
    	GL_FLOAT,           // type
    	GL_FALSE,           // normalized?
    	0,                  // stride
    	(void*)0            // array buffer offset
    );
    
    // 2nd attribute buffer : uv
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
    	1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    	2,                                // size
    	GL_FLOAT,                         // type
    	GL_FALSE,                         // normalized?
    	0,                                // stride
    	(void*)0                          // array buffer offset
    );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
    	2,                                // attribute
    	3,                                // size
    	GL_FLOAT,                         // type
    	GL_FALSE,                         // normalized?
    	0,                                // stride
    	(void*)0                          // array buffer offset
    );
    
    // glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    
    // Draw the triangles !
    glDrawElements(
    	GL_TRIANGLES,      // mode
    	indices.size(),    // count
    	GL_UNSIGNED_SHORT,   // type
    	(void*)0           // element array buffer offset
    );
    

    ////// Start of the rendering of the second object //////
    // for (int i = 0; i < 10; i++) {
    //   for (int j = 0; j < 10; j++) {
    //     glm::mat4 ModelMatrix2 = glm::mat4(1.0);
    //     ModelMatrix2 = glm::translate(ModelMatrix2, glm::vec3(30.0f + i * 30.0f, 0.0f, 40.0f + j * 40.0f));
    //     glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * ModelMatrix2;
    //     
    //     // Send our transformation to the currently bound shader, 
    //     // in the "MVP" uniform
    //     glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);
    //     glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix2[0][0]);
    //     
    //     
    //     // The rest is exactly the same as the first object
    //     
    //     // 1rst attribute buffer : vertices
    //     glEnableVertexAttribArray(0);
    //     glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    //     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //     
    //     // 2nd attribute buffer : UVs
    //     glEnableVertexAttribArray(1);
    //     glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    //     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //     
    //     // 3rd attribute buffer : normals
    //     glEnableVertexAttribArray(2);
    //     glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    //     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //     
    //     // Index buffer
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    //     
    //     // Draw the triangles !
    //     glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
    //   }
    // }
    ////// End of rendering of the second object //////

    UpdateGravity();
    spheres[0].Draw(ProjectionMatrix, ViewMatrix, MatrixID, ModelMatrixID);
    spheres[1].Draw(ProjectionMatrix, ViewMatrix, MatrixID, ModelMatrixID);
    spheres[0].UpdateMovement();
    spheres[1].UpdateMovement();
    spheres[0].DetectCollision(spheres[1]);













    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  
  } // Check if the ESC key was pressed or the window was closed
  while (
    glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0 
  );

  // Cleanup VBO and shader
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &uvbuffer);
  glDeleteBuffers(1, &normalbuffer);
  glDeleteBuffers(1, &elementbuffer);
  glDeleteProgram(programID);
  glDeleteTextures(1, &Texture);
  glDeleteVertexArrays(1, &VertexArrayID);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
