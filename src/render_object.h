#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

#include <glm/gtx/norm.hpp>
#include "controls.h"

struct AABB {
  glm::vec3 min, max;
};

extern bool over_ground;
extern glm::vec3 fall_speed;

class RenderObject {
  std::vector<glm::vec3> vertices_;
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
  GLuint plane_id_;
  GLuint CameraPositionID_;

  GLuint LightID_;
  GLuint MatrixID_;
  GLuint ViewMatrixID_;
  GLuint ModelMatrixID_;
  GLuint ModelView3x3MatrixID_;
  GLuint use_normals_id_;
  GLuint water_fog_id_;
  bool use_normals_;
  bool water_fog_;
  vec4 plane_;

 public:
  glm::vec3 position;

  RenderObject() {}
  RenderObject(const std::string& model, const std::string& texture, const std::string& normal_texture, const std::string& specular_texture, GLuint programID, bool use_normals) {
    programID_ = programID;
    LoadModel(model.c_str());
    Texture_ = loadBMP_custom(texture.c_str());
    TextureID_ = glGetUniformLocation(programID, "DiffuseTextureSampler");
    NormalTexture_ = loadBMP_custom(normal_texture.c_str());
    NormalTextureID_  = glGetUniformLocation(programID, "NormalTextureSampler");
    SpecularTexture_ = loadBMP_custom(specular_texture.c_str());
    SpecularTextureID_  = glGetUniformLocation(programID, "SpecularTextureSampler");
    CameraPositionID_ = glGetUniformLocation(programID, "cameraPosition");
    water_fog_id_ = glGetUniformLocation(programID, "water_fog");

    LightID_ = glGetUniformLocation(programID, "LightPosition_worldspace");
    MatrixID_ = glGetUniformLocation(programID, "MVP");
    ViewMatrixID_ = glGetUniformLocation(programID, "V");
    ModelMatrixID_ = glGetUniformLocation(programID, "M");
    ModelView3x3MatrixID_ = glGetUniformLocation(programID, "MV3x3");
    use_normals_id_ = glGetUniformLocation(programID, "use_normals");
    plane_id_ = glGetUniformLocation(programID, "plane");
    use_normals_ = use_normals;
    water_fog_ = false;

    plane_ = vec4(0, -1, 0, 10000);
  }

  void SetClipPlane(vec4 clip_plane) { plane_ = clip_plane; }
  void SetWaterFog(bool water_fog) { water_fog_ = water_fog; }

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

    // indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
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

  void Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
    glUseProgram(programID_);

    glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position);
    glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
    glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    glm::vec3 lightPos = glm::vec3(0,2000,0);
    glUniform3f(LightID_, lightPos.x, lightPos.y, lightPos.z);

    glUniformMatrix4fv(MatrixID_, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID_, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID_, 1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix3fv(ModelView3x3MatrixID_, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

    glUniform4fv(plane_id_, 1, (float*) &plane_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture_);
    glUniform1i(TextureID_, 0);

    glUniform1i(use_normals_id_, use_normals_);
    glUniform1i(water_fog_id_, water_fog_);

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

  bool TestCollisionAABB(glm::vec3* player_pos, glm::vec3 last_pos, AABB aabb, glm::vec3 triangle_points[]) {
    int min_x = player_pos->x - 0.5f;
    int max_x = player_pos->x + 0.5f;
    int min_y = player_pos->y - 20.0f;
    int max_y = player_pos->y + 1.0f;
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
    player_aabb.min= glm::vec3(player_pos->x - 0.5f, player_pos->y - 20.0f, player_pos->z - 0.5f);
    player_aabb.max = glm::vec3(player_pos->x + 0.5f, player_pos->y + 1.0f, player_pos->z + 0.5f);
  
    glm::vec3 player_aabb_center = (player_aabb.min + player_aabb.max) * 0.5f;
    glm::vec3 e = player_aabb.max - player_aabb_center;
    glm::vec3 triangle_normal = glm::normalize(glm::cross(triangle_points[1] - triangle_points[0], triangle_points[2] - triangle_points[1]));
    float d = triangle_normal.x * triangle_points[0].x + triangle_normal.y * triangle_points[0].y + triangle_normal.z * triangle_points[0].z;
  
    float r = e.x * fabs(triangle_normal.x) + e.y * fabs(triangle_normal.y) + e.z * fabs(triangle_normal.z);
    float s = glm::dot(triangle_normal, player_aabb_center) - d;
  
    if (fabs(s) <= r) {
      glm::vec3 new_pos = *player_pos + triangle_normal * (r - s);
      if (glm::distance(new_pos, last_pos) > glm::distance(*player_pos, last_pos)) return false;
  
      if (triangle_normal.y > 0.65f) over_ground = true;
      fall_speed.y += triangle_normal.y * 0.015f;
      if (fall_speed.y > 0.0f) fall_speed.y = 0.0f;
      
      *player_pos = new_pos;
      return true;
    }
    return false;
  }
  
  void TestCollision(glm::vec3* player_pos, glm::vec3 last_pos) {
    for (int i = 0; i < vertices_.size(); i += 3) {
      glm::vec3 points[3];
      points[0] = vertices_[i];
      points[1] = vertices_[i + 1];
      points[2] = vertices_[i + 2];
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
  
      TestCollisionAABB(player_pos, last_pos, aabb, points);
    }
  }

  void Clean() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteBuffers(1, &tangentbuffer);
    glDeleteBuffers(1, &bitangentbuffer);
    glDeleteProgram(programID_);
    glDeleteTextures(1, &Texture_);
    glDeleteTextures(1, &NormalTexture_);
    glDeleteTextures(1, &SpecularTexture_);
  }
};

#endif
