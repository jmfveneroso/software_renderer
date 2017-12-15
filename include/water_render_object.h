#ifndef WATER_RENDER_OBJECT_H
#define WATER_RENDER_OBJECT_H

#include <vector>
#include <string>
#include "bitmap.h"
#include "objloader.h"
#include "tangentspace.h"
#include "vbo_indexer.h"
#include <glm/gtx/norm.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#define WAVE_SPEED 0.005f

using namespace glm;

class WaterRenderObject {
  std::vector<glm::vec3> vertices_;
  std::vector<glm::vec3> indexed_vertices;
  std::vector<glm::vec2> indexed_uvs;
  std::vector<glm::vec3> indexed_normals; 
  std::vector<glm::vec3> indexed_tangents;
  std::vector<glm::vec3> indexed_bitangents;
  std::vector<unsigned short> indices; 
  GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer;
  GLuint tangentbuffer, bitangentbuffer;
  GLuint programID_;
  GLuint plane_id_;
  GLuint ReflectionTexture_;
  GLuint ReflectionTextureID_;
  GLuint RefractionTexture_;
  GLuint RefractionTextureID_;
  GLuint DuDvMapTexture_;
  GLuint DuDvMapTextureID_;
  GLuint NormalMapTexture_;
  GLuint NormalMapTextureID_;
  GLuint DepthMapID_;
  GLuint move_factor_id_;
  GLuint RefractionDepthTexture_;

  GLuint LightID_;
  GLuint MatrixID_;
  GLuint ViewMatrixID_;
  GLuint ModelMatrixID_;
  GLuint ModelView3x3MatrixID_;
  GLuint CameraPositionID_;
  vec4 plane_;

 public:
  static float move_factor;

  glm::vec3 position;

  WaterRenderObject() {}
  WaterRenderObject(const std::string& model, GLuint ReflectionTexture, GLuint RefractionTexture, const std::string& dudv_file, const std::string& normal_file, GLuint programID, GLuint RefractionDepthTexture) {
    programID_ = programID;
    LoadModel(model.c_str());

    RefractionDepthTexture_ = RefractionDepthTexture;
    ReflectionTexture_ = ReflectionTexture;
    ReflectionTextureID_ = glGetUniformLocation(programID, "ReflectionTextureSampler");
    RefractionTexture_ = RefractionTexture;
    RefractionTextureID_  = glGetUniformLocation(programID, "RefractionTextureSampler");
    DuDvMapTexture_ = loadBMP_custom(dudv_file.c_str());
    DuDvMapTextureID_  = glGetUniformLocation(programID, "dudvMap");
    NormalMapTexture_ = loadBMP_custom(normal_file.c_str());
    NormalMapTextureID_  = glGetUniformLocation(programID, "normalMap");
    DepthMapID_  = glGetUniformLocation(programID, "depthMap");

    LightID_ = glGetUniformLocation(programID, "LightPosition_worldspace");
    MatrixID_ = glGetUniformLocation(programID, "MVP");
    ViewMatrixID_ = glGetUniformLocation(programID, "V");
    ModelMatrixID_ = glGetUniformLocation(programID, "M");
    ModelView3x3MatrixID_ = glGetUniformLocation(programID, "MV3x3");
    plane_id_ = glGetUniformLocation(programID, "plane");
    move_factor_id_ = glGetUniformLocation(programID, "moveFactor");
    CameraPositionID_ = glGetUniformLocation(programID, "cameraPosition");

    plane_ = vec4(0, -1, 0, 10000);
  }

  static void UpdateMoveFactor(float seconds) {
    move_factor += WAVE_SPEED * seconds;
    move_factor = fmod(move_factor, 1);
  }

  void SetClipPlane(vec4 clip_plane) { plane_ = clip_plane; }

  void LoadModel(const char filename[]) {
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals; 
    loadOBJ(filename, vertices_, uvs, normals);

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    computeTangentBasis(
      vertices_, uvs, normals, // input
      tangents, bitangents    // output
    );

    indexVBO_TBN(
      vertices_, uvs, normals, tangents, bitangents, 
      indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_tangents, indexed_bitangents
    );
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

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

  void Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera_pos) {
    glUseProgram(programID_);

    glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position);
    ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0, 1.0, 1.0));
    glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
    glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    glm::vec3 lightPos = glm::vec3(0,200,0);
    glUniform3f(LightID_, lightPos.x, lightPos.y, lightPos.z);

    glUniformMatrix4fv(MatrixID_, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID_, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID_, 1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix3fv(ModelView3x3MatrixID_, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
    glUniform3fv(CameraPositionID_, 1, (float*) &camera_pos);

    glUniform4fv(plane_id_, 1, (float*) &plane_);
    glUniform1f(move_factor_id_, move_factor);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ReflectionTexture_);
    glUniform1i(ReflectionTextureID_, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, RefractionTexture_);
    glUniform1i(RefractionTextureID_, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, DuDvMapTexture_);
    glUniform1i(DuDvMapTextureID_, 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, NormalMapTexture_);
    glUniform1i(NormalMapTextureID_, 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, RefractionDepthTexture_);
    glUniform1i(DepthMapID_, 4);

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

  void Clean() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteBuffers(1, &tangentbuffer);
    glDeleteBuffers(1, &bitangentbuffer);
  }
};

#endif
