#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

#include <glm/gtx/norm.hpp>

class RenderObject {
 public:
   glm::vec3 position;

   std::vector<glm::vec3> indexed_vertices;
   std::vector<glm::vec2> indexed_uvs;
   std::vector<glm::vec3> indexed_normals; 
   std::vector<glm::vec3> indexed_tangents;
   std::vector<glm::vec3> indexed_bitangents;
   std::vector<unsigned short> indices; 
   GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer;
   GLuint tangentbuffer, bitangentbuffer;
   GLuint Texture_;
   GLuint TextureID_;
   GLuint NormalTexture_;
   GLuint NormalTextureID_;
   GLuint SpecularTexture_;
   GLuint SpecularTextureID_;
   GLuint programID_;

   RenderObject(const std::string& model, const std::string& texture, const std::string& normal_texture, const std::string& specular_texture, GLuint programID) {
     programID_ = programID;
     LoadModel(model.c_str());
     Texture_ = loadBMP_custom(texture.c_str());
     TextureID_ = glGetUniformLocation(programID, "DiffuseTextureSampler");
     NormalTexture_ = loadBMP_custom(normal_texture.c_str());
     NormalTextureID_  = glGetUniformLocation(programID, "NormalTextureSampler");
     SpecularTexture_ = loadBMP_custom(specular_texture.c_str());
     SpecularTextureID_  = glGetUniformLocation(programID, "SpecularTextureSampler");
   }

   void LoadModel(const char filename[]) {
     std::vector<glm::vec3> vertices;
     std::vector<glm::vec2> uvs;
     std::vector<glm::vec3> normals; 
     loadOBJ(filename, vertices, uvs, normals);

     std::vector<glm::vec3> tangents;
     std::vector<glm::vec3> bitangents;
     computeTangentBasis(
       vertices, uvs, normals, // input
       tangents, bitangents    // output
     );

     // indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
     indexVBO_TBN(
       vertices, uvs, normals, tangents, bitangents, 
       indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_tangents, indexed_bitangents
     );
     
     glGenBuffers(1, &vertexbuffer);
     glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
     glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
     
     glGenBuffers(1, &uvbuffer);
     glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
     glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
     
     glGenBuffers(1, &normalbuffer);
     glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
     glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

     glGenBuffers(1, &tangentbuffer);
     glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
     glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);
     
     glGenBuffers(1, &bitangentbuffer);
     glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
     glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);
     
     glGenBuffers(1, &elementbuffer);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);
   }

   void Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::mat3 ModelView3x3Matrix, GLuint MatrixID, GLuint ViewMatrixID, GLuint ModelMatrixID, GLuint ModelView3x3MatrixID) {
     glUseProgram(programID_);

     glm::mat4 ModelMatrix = glm::mat4(1.0);
     ModelMatrix = glm::translate(ModelMatrix, position);
     glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
     
     // Send our transformation to the currently bound shader, 
     // in the "MVP" uniform
     glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
     glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
     glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
     glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
     
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, Texture_);
     glUniform1i(TextureID_, 0);

     // Bind our normal texture in Texture Unit 1
     glActiveTexture(GL_TEXTURE1);
     glBindTexture(GL_TEXTURE_2D, NormalTexture_);
     // Set our "NormalTextureSampler" sampler to use Texture Unit 1
     glUniform1i(NormalTextureID_, 1);

     // Bind our specular texture in Texture Unit 2
     glActiveTexture(GL_TEXTURE2);
     glBindTexture(GL_TEXTURE_2D, SpecularTexture_);
     // Set our "SpecularTextureSampler" sampler to use Texture Unit 2
     glUniform1i(SpecularTextureID_, 2);

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

     // 4th attribute buffer : tangents
     glEnableVertexAttribArray(3);
     glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
     glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
     
     // 5th attribute buffer : bitangents
     glEnableVertexAttribArray(4);
     glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
     glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
     
     // Index buffer
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
     
     // Draw the triangles !
     glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

     glDisableVertexAttribArray(0);
     glDisableVertexAttribArray(1);
     glDisableVertexAttribArray(2);
     glDisableVertexAttribArray(3);
     glDisableVertexAttribArray(4);
   }
};

#endif
