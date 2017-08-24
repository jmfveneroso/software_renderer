#ifndef SPHERE_H
#define SPHERE_H

#include <glm/gtx/norm.hpp>

class Sphere {
 public:
   glm::vec3 position;
   glm::vec3 velocity;

   static std::vector<glm::vec3> indexed_vertices;
   static std::vector<glm::vec2> indexed_uvs;
   static std::vector<glm::vec3> indexed_normals; 
   static std::vector<unsigned short> indices; 
   static GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer;

   Sphere() : position(0.0f, 0.0f, 0.0f) {}

   Sphere(float x, float y, float z, float vx, float vy, float vz) : 
     position(x, y, z), velocity(vx, vy, vz) {
   }

   static void SetModel(const char filename[]) {
     std::vector<glm::vec3> vertices;
     std::vector<glm::vec2> uvs;
     std::vector<glm::vec3> normals; 
     loadOBJ(filename, vertices, uvs, normals);
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
   }

   void Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, GLuint MatrixID, GLuint ModelMatrixID) {
     glm::mat4 ModelMatrix = glm::mat4(1.0);
     ModelMatrix = glm::translate(ModelMatrix, position);
     glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
     
     // Send our transformation to the currently bound shader, 
     // in the "MVP" uniform
     glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
     glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
     
     // 1rst attribute buffer : vertices
     glEnableVertexAttribArray(0);
     glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
     
     // 2nd attribute buffer : UVs
     glEnableVertexAttribArray(1);
     glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
     
     // 3rd attribute buffer : normals
     glEnableVertexAttribArray(2);
     glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
     
     // Index buffer
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
     
     // Draw the triangles !
     glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
   }

   void DetectCollision(Sphere& sphere) {
     float radius_distance_squared = 4.0f;
     float center_distance = glm::length2(position - sphere.position);

     if (center_distance < radius_distance_squared) {
       printf("collision \n");
       sphere.velocity = -sphere.velocity;
       velocity = -velocity;
     } else {
     }
   }

   void UpdateMovement() {
     position += velocity;
   }
};

std::vector<glm::vec3> Sphere::indexed_vertices = std::vector<glm::vec3>();
std::vector<glm::vec2> Sphere::indexed_uvs = std::vector<glm::vec2>();
std::vector<glm::vec3> Sphere::indexed_normals = std::vector<glm::vec3>(); 
std::vector<unsigned short> Sphere::indices = std::vector<unsigned short>(); 
GLuint Sphere::vertexbuffer = 0;
GLuint Sphere::uvbuffer = 0;
GLuint Sphere::normalbuffer = 0;
GLuint Sphere::elementbuffer = 0;

#endif
