#ifndef CUBE_H
#define CUBE_H

#include <glm/gtx/norm.hpp>

class Cube {
 public:
   glm::vec3 position;

   static std::vector<glm::vec3> indexed_vertices;
   static std::vector<glm::vec2> indexed_uvs;
   static std::vector<glm::vec3> indexed_normals; 
   static std::vector<unsigned short> indices; 
   static GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer;

   Cube() : position(0.0f, 0.0f, 0.0f) {}

   Cube(float x, float y, float z) : 
     position(x, y, z) {
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

   bool TestCollision(glm::vec3* player_pos, glm::vec3 last_pos) {
     int min_x = player_pos->x - 1.0f;
     int max_x = player_pos->x + 1.0f;
     int min_y = player_pos->y - 4.0f;
     int max_y = player_pos->y + 4.0f;
     int min_z = player_pos->z - 1.0f;
     int max_z = player_pos->z + 1.0f;

     int cube_min_x = this->position.x - 1.0f;
     int cube_max_x = this->position.x + 1.0f;
     int cube_min_y = this->position.y - 1.0f;
     int cube_max_y = this->position.y + 1.0f;
     int cube_min_z = this->position.z - 1.0f;
     int cube_max_z = this->position.z + 1.0f;

     if (max_x < cube_min_x) return false;
     if (min_x > cube_max_x) return false;
     if (max_y < cube_min_y) return false;
     if (min_y > cube_max_y) return false;
     if (max_z < cube_min_z) return false;
     if (min_z > cube_max_z) return false;


     glm::vec3 normal = last_pos - this->position;
     if (fabs(normal.x) >= fabs(normal.y) && fabs(normal.x) >= fabs(normal.z)) {
       if (normal.x < 0.0f) player_pos->x = cube_min_x - 2.0f;
       else player_pos->x = cube_max_x + 2.0f;
     }
     else if (fabs(normal.y) >= fabs(normal.x) && fabs(normal.y) >= fabs(normal.z)) {
       if (fall_speed.y > 0.0f) {
       } else {
         if (normal.y < 0.0f) player_pos->y = cube_min_y - 5.0f;
         else {
           player_pos->y = cube_max_y + 5.0f;
           fall_speed = glm::vec3(0, 0.0, 0);
         }
       }
     }
     else {
       if (normal.z < 0.0f) player_pos->z = cube_min_z - 2.0f;
       else player_pos->z = cube_max_z + 1.00001f;
     }

     return true;
   }
};

std::vector<glm::vec3> Cube::indexed_vertices = std::vector<glm::vec3>();
std::vector<glm::vec2> Cube::indexed_uvs = std::vector<glm::vec2>();
std::vector<glm::vec3> Cube::indexed_normals = std::vector<glm::vec3>(); 
std::vector<unsigned short> Cube::indices = std::vector<unsigned short>(); 
GLuint Cube::vertexbuffer = 0;
GLuint Cube::uvbuffer = 0;
GLuint Cube::normalbuffer = 0;
GLuint Cube::elementbuffer = 0;

#endif
