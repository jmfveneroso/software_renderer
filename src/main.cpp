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
#include "cube.h"
#include "render_object.h"

GLuint concurrentProgramID;
std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;
std::vector<unsigned short> indices;
std::vector<glm::vec3> indexed_vertices;
std::vector<glm::vec2> indexed_uvs;
std::vector<glm::vec3> indexed_normals;

void DrawTriangle(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, GLuint MatrixID, glm::vec3 vectors[]) {
  GLfloat buffer[] = {
    vectors[0].x, vectors[0].y, vectors[0].z,
    vectors[1].x, vectors[1].y, vectors[1].z,
    vectors[2].x, vectors[2].y, vectors[2].z
  };

  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);

  glUseProgram(concurrentProgramID);
  glm::mat4 ModelMatrix = glm::mat4(1.0);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  // Send our transformation to the currently bound shader, 
  // in the "MVP" uniform
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
  // glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
  
  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  
  // Draw the triangles !
  glDrawArrays(GL_LINE_LOOP, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
  glDisableVertexAttribArray(0);
}

struct AABB {
  glm::vec3 min, max;
};

bool TestCollisionAABB(glm::vec3* player_pos, glm::vec3 last_pos, AABB aabb, glm::vec3 triangle_points[]) {
  int min_x = player_pos->x - 0.5f;
  int max_x = player_pos->x + 0.5f;
  int min_y = player_pos->y - 4.0f;
  int max_y = player_pos->y + 4.0f;
  int min_z = player_pos->z - 0.5f;
  int max_z = player_pos->z + 0.5f;

  int cube_min_x = aabb.min.x;
  int cube_max_x = aabb.max.x;
  int cube_min_y = aabb.min.y;
  int cube_max_y = aabb.max.y;
  int cube_min_z = aabb.min.z;
  int cube_max_z = aabb.max.z;

  if (max_x < cube_min_x) return false;
  if (min_x > cube_max_x) return false;
  if (max_y < cube_min_y) return false;
  if (min_y > cube_max_y) return false;
  if (max_z < cube_min_z) return false;
  if (min_z > cube_max_z) return false;

  AABB player_aabb;
  player_aabb.min= glm::vec3(player_pos->x - 0.5f, player_pos->y - 4.0f, player_pos->z - 0.5f);
  player_aabb.max = glm::vec3(player_pos->x + 0.5f, player_pos->y + 4.0f, player_pos->z + 0.5f);

  glm::vec3 player_aabb_center = (player_aabb.min + player_aabb.max) * 0.5f;
  glm::vec3 e = player_aabb.max - player_aabb_center;
  glm::vec3 triangle_normal = glm::normalize(glm::cross(triangle_points[1] - triangle_points[0], triangle_points[2] - triangle_points[1]));
  float d = triangle_normal.x * triangle_points[0].x + triangle_normal.y * triangle_points[0].y + triangle_normal.z * triangle_points[0].z;

  float r = e.x * fabs(triangle_normal.x) + e.y * fabs(triangle_normal.y) + e.z * fabs(triangle_normal.z);
  float s = glm::dot(triangle_normal, player_aabb_center) - d;

  if (fabs(s) <= r) {
    glm::vec3 new_pos = *player_pos + triangle_normal * (r - s);
    if (glm::distance(new_pos, last_pos) > glm::distance(*player_pos, last_pos)) return false;

    // if (triangle_normal.y > 0.0) over_ground = true;
    if (triangle_normal.y > 0.65f) over_ground = true;
    fall_speed.y += triangle_normal.y * 0.015f;
    if (fall_speed.y > 0.0f) fall_speed.y = 0.0f;
    
    *player_pos = new_pos;
    return true;
    // intersection;
  }
  return false;
}

void TestCollision(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, GLuint MatrixID, glm::vec3* player_pos, glm::vec3 last_pos) {
  for (int i = 0; i < vertices.size(); i += 3) {
    glm::vec3 points[3];
    points[0] = vertices[i];
    points[1] = vertices[i + 1];
    points[2] = vertices[i + 2];
    // DrawTriangle(ProjectionMatrix, ViewMatrix, MatrixID, points);

    AABB aabb;
    aabb.min = glm::vec3(999999.0f, 999999.0f, 999999.0f);
    aabb.max = glm::vec3(-999999.0f, -999999.0f, -999999.0f);
    for (int j = 0; j < 3; j++) {
      if (points[j].x < aabb.min.x) aabb.min.x = points[j].x;
      if (points[j].x > aabb.max.x) aabb.max.x = points[j].x;
      if (points[j].y < aabb.min.y) aabb.min.y = points[j].y;
      if (points[j].y > aabb.max.y) aabb.max.y = points[j].y;
      if (points[j].z < aabb.min.z) aabb.min.z = points[j].z;
      if (points[j].z > aabb.max.z) aabb.max.z = points[j].z;
    }

    // printf("aabb %f %f %f %f %f %f\n", aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
    if (TestCollisionAABB(player_pos, last_pos, aabb, points)) {
      // glm::vec3 x = glm::normalize(glm::cross(points[1] - points[0], points[2] - points[1]));
      // *player_pos += x * 10.0f;
    }
  }
}

GLuint CreateMvpMatrix(GLuint programID, glm::mat4* MVP) {
  GLuint MatrixID = glGetUniformLocation(programID, "MVP");

  // Projection matrix.
  glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 2000.0f);

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
  GLuint skyProgramID = LoadShaders("shaders/vshade", "shaders/fshade_sky");
  concurrentProgramID = LoadShaders("shaders/vshade2", "shaders/fshade2");
  
  glm::mat4 MVP;
  GLuint MatrixID = CreateMvpMatrix(programID, &MVP); 
  GLuint ConcurrentMatrixID = CreateMvpMatrix(concurrentProgramID, &MVP); 
  GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
  GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

  GLuint Texture = loadBMP_custom("textures/medium_terrain.bmp");
  GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

  GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer;

  // bool res = loadOBJ("res/suzanne.obj", vertices, uvs, normals);
  // bool res = loadOBJ("res/basic_level.obj", vertices, uvs, normals);
  bool res = loadOBJ("res/medium_terrain.obj", vertices, uvs, normals);
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

  RenderObject sky_dome("res/skydome.obj", "textures/skydome.bmp", skyProgramID);

  Cube::SetModel("res/cube.obj");
  Cube cube(0.0f, 2.0f, -20.0f);

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

    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    glm::vec3 lightPos = glm::vec3(0,200,0);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    sky_dome.position = position;
    sky_dome.Draw(ProjectionMatrix, ViewMatrix, MatrixID, ModelMatrixID);

    glUseProgram(programID);
    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    // glUniform1i(TextureID, 0);

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

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

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

    // spheres[0].Draw(ProjectionMatrix, ViewMatrix, MatrixID, ModelMatrixID);
    // spheres[1].Draw(ProjectionMatrix, ViewMatrix, MatrixID, ModelMatrixID);
    // spheres[0].UpdateMovement();
    // spheres[1].UpdateMovement();
    // spheres[0].DetectCollision(spheres[1]);

    cube.Draw(ProjectionMatrix, ViewMatrix, MatrixID, ModelMatrixID);

    glm::vec3 last_pos = position;
    UpdatePlayerPos();
    UpdateGravity();

    // AABB aabb;
    // aabb.min.x = cube.position.x - 1.0f;
    // aabb.max.x = cube.position.x + 1.0f;
    // aabb.min.y = cube.position.y - 1.0f;
    // aabb.max.y = cube.position.y + 1.0f;
    // aabb.min.z = cube.position.z - 1.0f;
    // aabb.max.z = cube.position.z + 1.0f;
    
    // TestCollisionAABB(&position, last_pos, aabb);
    TestCollision(ProjectionMatrix, ViewMatrix, ConcurrentMatrixID, &position, last_pos);
    computeMatricesFromInputs();

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
