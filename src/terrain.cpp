#include "terrain.hpp"

namespace Sibyl {

Terrain::Terrain(
  std::shared_ptr<Player> player,
  Shader shader, 
  GLuint diffuse_texture_id, 
  GLuint normal_texture_id, 
  GLuint specular_texture_id
) : player_(player),
    shader_(shader),
    diffuse_texture_id_(diffuse_texture_id), 
    normal_texture_id_(normal_texture_id), 
    specular_texture_id_(specular_texture_id),
    position_(glm::vec3(0.0, 0.0, 0.0)) {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &normal_buffer_);
  glGenBuffers(1, &tangent_buffer_);
  glGenBuffers(1, &bitangent_buffer_);
  glGenBuffers(1, &element_buffer_);

  GenerateTerrain(true);
  UpdateQuads();
}


float Terrain::GetHeight(float x, float y) {
  return 5 * noise_.noise(1000 + x * 0.01, 1000 + y * 0.01) + 
         100 * noise_.noise(x * 0.0003, y * 0.0003) +
         // 2000 * noise_.noise(x * 0.0001, y * 0.0001) + 
         500 * noise_.noise(100 + x * 0.0001, 100 + y * 0.0001) +
         500 * noise_.noise(200 + x * 0.0001, 200 + y * 0.0001) + 
         500 * noise_.noise(300 + x * 0.0001, 300 + y * 0.0001) + 
         500 * noise_.noise(400 + x * 0.0001, 400 + y * 0.0001);
  // float factor = 0.005;
  // float factor2 = 50;
  // return factor2 * (sin(x * factor) + sin(y * factor)); 
}

unsigned int Terrain::AddVertex(float x, float y, float u, float v) {
  float side = 100;

  float x_offset = player_->position().x;
  float y_offset = player_->position().z;

  x = x_offset + x * side + u * side;
  y = y_offset + y * side + v * side;

  float height = GetHeight(x, y);
  indexed_vertices_.push_back(glm::vec3(x, height, y));
  indexed_uvs_.push_back(glm::vec2(u, v));
  indexed_normals_.push_back(glm::vec3(0, 1, 0));
  indexed_tangents_.push_back(glm::vec3(0, 0, 0));
  indexed_bitangents_.push_back(glm::vec3(0, 0, 0));
  return indexed_vertices_.size() - 1;
}

unsigned int Terrain::AddVertexToQuad(TerrainQuad* quad, float x, float y, float u, float v, glm::vec3 normal) {
  x = x + u * TILE_SIZE;
  y = y + v * TILE_SIZE;

  float height = GetHeight(x, y);
  quad->indexed_vertices.push_back(glm::vec3(x, height, y));
  quad->indexed_uvs.push_back(glm::vec2(u, v));
  quad->indexed_normals.push_back(normal);
  quad->indexed_tangents.push_back(glm::vec3(0, 0, 0));
  quad->indexed_bitangents.push_back(glm::vec3(0, 0, 0));
  return quad->indexed_vertices.size() - 1;
}

void Terrain::UpdateQuad(int x1, int y1) {
  for (int i = 0; i < NUM_QUADS; i++) {
    TerrainQuad& quad = quads_[i];
    if (quad.x == x1 && quad.y == y1 && !quad.empty) return;
  }

  for (int i = 0; i < NUM_QUADS; i++) {
    TerrainQuad& quad = quads_[i];
    if (!quad.empty) continue;
  
    quad.x = x1;     
    quad.y = y1;     
    quad.empty = false;

    quad.indices.clear();
    quad.indexed_vertices.clear();
    quad.indexed_uvs.clear();
    quad.indexed_normals.clear();
    quad.indexed_tangents.clear();
    quad.indexed_bitangents.clear();

    for (int x2 = 0; x2 < QUAD_SIZE; x2++) {
      for (int y2 = 0; y2 < QUAD_SIZE; y2++) {
        float x = x1 * QUAD_SIZE * TILE_SIZE + x2 * TILE_SIZE;
        float y = y1 * QUAD_SIZE * TILE_SIZE + y2 * TILE_SIZE;

        glm::vec3 v_1 = glm::vec3(x + 0 * TILE_SIZE, GetHeight(x + 0 * TILE_SIZE, y + 0 * TILE_SIZE), y + 0 * TILE_SIZE);
        glm::vec3 v_2 = glm::vec3(x + 0 * TILE_SIZE, GetHeight(x + 0 * TILE_SIZE, y + 1 * TILE_SIZE), y + 1 * TILE_SIZE);
        glm::vec3 v_3 = glm::vec3(x + 1 * TILE_SIZE, GetHeight(x + 1 * TILE_SIZE, y + 1 * TILE_SIZE), y + 1 * TILE_SIZE);
        glm::vec3 a = v_2 - v_1;
        glm::vec3 b = v_3 - v_1;
        glm::vec3 normal = glm::cross(a, b);

        unsigned int v1 = AddVertexToQuad(&quad, x, y, 0, 0, normal);
        unsigned int v2 = AddVertexToQuad(&quad, x, y, 0, 1, normal);
        unsigned int v3 = AddVertexToQuad(&quad, x, y, 1, 1, normal);
        unsigned int v4 = AddVertexToQuad(&quad, x, y, 1, 0, normal);

        quad.indices.push_back(v1);
        quad.indices.push_back(v2);
        quad.indices.push_back(v3);
        quad.indices.push_back(v1);
        quad.indices.push_back(v3);
        quad.indices.push_back(v4);
      }
    }

    if (!quad.initialized) {
      glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
      glBufferData(GL_ARRAY_BUFFER, quad.indexed_vertices.size() * sizeof(glm::vec3), &quad.indexed_vertices[0], GL_DYNAMIC_DRAW);
      
      glBindBuffer(GL_ARRAY_BUFFER, quad.uv_buffer);
      glBufferData(GL_ARRAY_BUFFER, quad.indexed_uvs.size() * sizeof(glm::vec2), &quad.indexed_uvs[0], GL_DYNAMIC_DRAW);
      
      glBindBuffer(GL_ARRAY_BUFFER, quad.normal_buffer);
      glBufferData(GL_ARRAY_BUFFER, quad.indexed_normals.size() * sizeof(glm::vec3), &quad.indexed_normals[0], GL_DYNAMIC_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, quad.tangent_buffer);
      glBufferData(GL_ARRAY_BUFFER, quad.indexed_tangents.size() * sizeof(glm::vec3), &quad.indexed_tangents[0], GL_DYNAMIC_DRAW);
      
      glBindBuffer(GL_ARRAY_BUFFER, quad.bitangent_buffer);
      glBufferData(GL_ARRAY_BUFFER, quad.indexed_bitangents.size() * sizeof(glm::vec3), &quad.indexed_bitangents[0], GL_DYNAMIC_DRAW);
      
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.element_buffer);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, quad.indices.size() * sizeof(unsigned int), &quad.indices[0], GL_DYNAMIC_DRAW);
      quad.initialized = true;
    } else {
      glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
      glBufferSubData(GL_ARRAY_BUFFER, 0, quad.indexed_vertices.size() * sizeof(glm::vec3), &quad.indexed_vertices[0]);
      
      glBindBuffer(GL_ARRAY_BUFFER, quad.uv_buffer);
      glBufferSubData(GL_ARRAY_BUFFER, 0, quad.indexed_uvs.size() * sizeof(glm::vec2), &quad.indexed_uvs[0]);
      
      glBindBuffer(GL_ARRAY_BUFFER, quad.normal_buffer);
      glBufferSubData(GL_ARRAY_BUFFER, 0, quad.indexed_normals.size() * sizeof(glm::vec3), &quad.indexed_normals[0]);

      glBindBuffer(GL_ARRAY_BUFFER, quad.tangent_buffer);
      glBufferSubData(GL_ARRAY_BUFFER, 0, quad.indexed_tangents.size() * sizeof(glm::vec3), &quad.indexed_tangents[0]);
      
      glBindBuffer(GL_ARRAY_BUFFER, quad.bitangent_buffer);
      glBufferSubData(GL_ARRAY_BUFFER, 0, quad.indexed_bitangents.size() * sizeof(glm::vec3), &quad.indexed_bitangents[0]);
      
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.element_buffer);
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, quad.indices.size() * sizeof(unsigned int), &quad.indices[0]);
    }
    break;
  }
}

void Terrain::UpdateQuads() {
  int center_x = player_->position().x / (QUAD_SIZE * TILE_SIZE);
  int center_y = player_->position().z / (QUAD_SIZE * TILE_SIZE);

  // Delete distant quads.
  for (int i = 0; i < NUM_QUADS; i++) {
    TerrainQuad& quad = quads_[i];

    int max_distance = BIG_QUAD_SIDE / 2;
    if (
      abs(quad.x - center_x) > max_distance || 
      abs(quad.y - center_y) > max_distance
    ) {
      quad.empty = true;
    }
  }

  for (int i = -BIG_QUAD_SIDE / 2; i <= BIG_QUAD_SIDE / 2; i++) {
    for (int j = -BIG_QUAD_SIDE / 2; j <= BIG_QUAD_SIDE / 2; j++) {
      int x = center_x + i;
      int y = center_y + j;
      UpdateQuad(center_x + i, center_y + j);
    }
  }
}

void Terrain::GenerateTerrain(bool full) {
  return;
  indices_.clear();
  indexed_vertices_.clear();
  indexed_uvs_.clear();
  indexed_normals_.clear();
  indexed_tangents_.clear();
  indexed_bitangents_.clear();

  int grid_size = 100;
  for (int x = -grid_size; x <= grid_size - 1; x++) {
    for (int y = -grid_size; y <= grid_size - 1; y++) {
      unsigned int v1 = AddVertex(x, y, 0, 0);
      unsigned int v2 = AddVertex(x, y, 0, 1);
      unsigned int v3 = AddVertex(x, y, 1, 1);
      unsigned int v4 = AddVertex(x, y, 1, 0);

      indices_.push_back(v1);
      indices_.push_back(v2);
      indices_.push_back(v3);
      indices_.push_back(v1);
      indices_.push_back(v3);
      indices_.push_back(v4);
    }
  }

  if (full) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices_.size() * sizeof(glm::vec3), &indexed_vertices_[0], GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs_.size() * sizeof(glm::vec2), &indexed_uvs_[0], GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals_.size() * sizeof(glm::vec3), &indexed_normals_[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, tangent_buffer_);
    glBufferData(GL_ARRAY_BUFFER, indexed_tangents_.size() * sizeof(glm::vec3), &indexed_tangents_[0], GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, bitangent_buffer_);
    glBufferData(GL_ARRAY_BUFFER, indexed_bitangents_.size() * sizeof(glm::vec3), &indexed_bitangents_[0], GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_DYNAMIC_DRAW);
  } else {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, indexed_vertices_.size() * sizeof(glm::vec3), &indexed_vertices_[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, indexed_uvs_.size() * sizeof(glm::vec2), &indexed_uvs_[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, indexed_normals_.size() * sizeof(glm::vec3), &indexed_normals_[0]);

    glBindBuffer(GL_ARRAY_BUFFER, tangent_buffer_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, indexed_tangents_.size() * sizeof(glm::vec3), &indexed_tangents_[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, bitangent_buffer_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, indexed_bitangents_.size() * sizeof(glm::vec3), &indexed_bitangents_[0]);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices_.size() * sizeof(unsigned int), &indices_[0]);
  }
}

void Terrain::DrawQuads(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());

  // Uniforms.
  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader_.GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

  glm::vec3 lightPos = glm::vec3(0, 2000, 0);
  glUniform3f(shader_.GetUniformId("LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);

  glm::vec4 plane = glm::vec4(0, -1, 0, 10000);
  glUniform4fv(shader_.GetUniformId("plane"), 1, (float*) &plane);
  glUniform1i(shader_.GetUniformId("use_normals"), false);
  glUniform1i(shader_.GetUniformId("water_fog"), false);

  // Textures.
  shader_.BindTexture("DiffuseTextureSampler", diffuse_texture_id_);
  shader_.BindTexture("NormalTextureSampler", normal_texture_id_);
  shader_.BindTexture("SpecularTextureSampler", specular_texture_id_);

  for (int i = 0; i < NUM_QUADS; i++) {
    TerrainQuad& quad = quads_[i];

    // Buffers.
    shader_.BindBuffer(quad.vertex_buffer, 0, 3);
    shader_.BindBuffer(quad.uv_buffer, 1, 2);
    shader_.BindBuffer(quad.normal_buffer, 2, 3);
    shader_.BindBuffer(quad.tangent_buffer, 3, 3);
    shader_.BindBuffer(quad.bitangent_buffer, 4, 3);
    
    // Index buffer.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.element_buffer);

    glDrawElements(GL_TRIANGLES, quad.indices.size(), GL_UNSIGNED_INT, (void*) 0);
  }
  shader_.Clear();
}

void Terrain::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  DrawQuads(ProjectionMatrix, ViewMatrix, camera);
  return;

  // CreateTiles();
  glUseProgram(shader_.program_id());

  // Uniforms.
  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader_.GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

  glm::vec3 lightPos = glm::vec3(0, 2000, 0);
  glUniform3f(shader_.GetUniformId("LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);

  glm::vec4 plane = glm::vec4(0, -1, 0, 10000);
  glUniform4fv(shader_.GetUniformId("plane"), 1, (float*) &plane);
  glUniform1i(shader_.GetUniformId("use_normals"), false);
  glUniform1i(shader_.GetUniformId("water_fog"), false);

  // Textures.
  shader_.BindTexture("DiffuseTextureSampler", diffuse_texture_id_);
  shader_.BindTexture("NormalTextureSampler", normal_texture_id_);
  shader_.BindTexture("SpecularTextureSampler", specular_texture_id_);

  // Buffers.
  shader_.BindBuffer(vertex_buffer_, 0, 3);
  shader_.BindBuffer(uv_buffer_, 1, 2);
  shader_.BindBuffer(normal_buffer_, 2, 3);
  shader_.BindBuffer(tangent_buffer_, 3, 3);
  shader_.BindBuffer(bitangent_buffer_, 4, 3);
  
  // Index buffer.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);

  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*) 0);
  // Clean();
  shader_.Clear();
}

void Terrain::Clean() {
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteBuffers(1, &uv_buffer_);
  glDeleteBuffers(1, &normal_buffer_);
  glDeleteBuffers(1, &element_buffer_);
  glDeleteBuffers(1, &tangent_buffer_);
  glDeleteBuffers(1, &bitangent_buffer_);
}

} // End of namespace.
