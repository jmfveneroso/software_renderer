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
    position_(glm::vec3(0.0, 0.0, 0.0)),
    last_center_x_(9999),
    last_center_y_(9999) {
  UpdateQuads();
  glGenBuffers(1, &tile_vertex_buffers_[0]);
  glGenBuffers(1, &tile_uv_buffers_[0]);
  glGenBuffers(1, &tile_normal_buffers_[0]);
  glGenBuffers(1, &tile_index_buffers_[0]);

  glm::vec3 vertices[4];
  vertices[0] = glm::vec3(0, 0, 0);
  vertices[1] = glm::vec3(0, 0, 32);
  vertices[2] = glm::vec3(32, 0, 32);
  vertices[3] = glm::vec3(32, 0, 0);

  glm::vec2 uvs[4];
  uvs[0] = glm::vec2(0, 0);
  uvs[1] = glm::vec2(0, 1);
  uvs[2] = glm::vec2(1, 1);
  uvs[3] = glm::vec2(1, 0);

  glm::vec3 normals[4];
  normals[0] = glm::vec3(0, 1, 0);
  normals[1] = glm::vec3(0, 1, 0);
  normals[2] = glm::vec3(0, 1, 0);
  normals[3] = glm::vec3(0, 1, 0);

  unsigned int indices[6];
  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;
  indices[3] = 0;
  indices[4] = 2;
  indices[5] = 3;

  glBindBuffer(GL_ARRAY_BUFFER, tile_vertex_buffers_[0]);
  glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, tile_uv_buffers_[0]);
  glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, tile_normal_buffers_[0]);
  glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
      
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tile_index_buffers_[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  data = new float[257 * 257];
  for (int i = 0; i < 257; i++) {
    for (int j = 0; j < 257; j++) {
      data[i*257 + j] = (GetHeight(j*32, i*32));
    }
  }

  glGenTextures(1, &height_map_);
  glBindTexture(GL_TEXTURE_2D, height_map_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 257, 257, 0, GL_RED, GL_FLOAT, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

float Terrain::GetHeight(float x, float y) {
  return 32 * noise_.noise(1000 + x * 0.01, 1000 + y * 0.01) +
         200 * noise_.noise(x * 0.001, y * 0.001);
         // 2000 * noise_.noise(1000 + x * 0.0001, 1000 + y * 0.0001);
         // 100 * noise_.noise(x * 0.002, y * 0.002);
}

// void Terrain::LoadTerrain(int x1, int y1, int center_x, int center_y) {
//   for (int i = 0; i < NUM_QUADS; i++) {
//     TerrainQuad& quad = quads_[i];
//     for (int x2 = 0; x2 < QUAD_SIZE; x2++) {
//       for (int y2 = 0; y2 < QUAD_SIZE; y2++) {
//         float x = x1 * QUAD_SIZE * TILE_SIZE + x2 * TILE_SIZE;
//         float y = y1 * QUAD_SIZE * TILE_SIZE + y2 * TILE_SIZE;
// 
//         glm::vec3 v_1 = glm::vec3(x + 0 * TILE_SIZE, GetHeight(x + 0 * TILE_SIZE, y + 0 * TILE_SIZE), y + 0 * TILE_SIZE);
//         glm::vec3 v_2 = glm::vec3(x + 0 * TILE_SIZE, GetHeight(x + 0 * TILE_SIZE, y + n * TILE_SIZE), y + n * TILE_SIZE);
//         glm::vec3 v_3 = glm::vec3(x + n * TILE_SIZE, GetHeight(x + n * TILE_SIZE, y + n * TILE_SIZE), y + n * TILE_SIZE);
//         glm::vec3 v_4 = glm::vec3(x + n * TILE_SIZE, GetHeight(x + n * TILE_SIZE, y + 0 * TILE_SIZE), y + 0 * TILE_SIZE);
// 
//         glm::vec3 a = v_2 - v_1;
//         glm::vec3 b = v_3 - v_1;
//         glm::vec3 normal = glm::cross(a, b);
// 
//         int i = 4 * (x2 * (QUAD_SIZE/n) + y2);
//         unsigned int v1 = AddVertexToQuad(&quad, i + 0, v_1, x2 * n,     y2 * n,     normal);
//         unsigned int v2 = AddVertexToQuad(&quad, i + 1, v_2, x2 * n,     y2 * n + n, normal);
//         unsigned int v3 = AddVertexToQuad(&quad, i + 2, v_3, x2 * n + n, y2 * n + n, normal);
//         unsigned int v4 = AddVertexToQuad(&quad, i + 3, v_4, x2 * n + n, y2 * n,     normal);
//       }
//     }
//   }
// }

unsigned int Terrain::AddVertexToQuad(
  TerrainQuad* quad, 
  int i,
  glm::vec3 pos,
  float u, float v, 
  glm::vec3 normal
) {
  quad->indexed_vertices  [i] = pos;
  quad->indexed_uvs       [i] = glm::vec2(u, v);
  quad->indexed_normals   [i] = normal;
  quad->indexed_tangents  [i] = glm::vec3(0, 0, 0);
  quad->indexed_bitangents[i] = glm::vec3(0, 0, 0);
  return i;
}

void Terrain::UpdateQuad(int x1, int y1, int center_x, int center_y) {
  bool worse_up=false, worse_down=false, worse_left=false, worse_right=false;
  int i = x1; int j = y1;
  if (i == j && i < 0) { // top-left corner
    worse_up = true; worse_left = true;
  }
  else if (i == j && i > 0) { // bottom-right corner
    worse_down = true; worse_right = true;
  }
  else if (i == j && i == 0) { // center
    worse_up = true; worse_down = true; worse_left = true; worse_right = true;
  }
  else if (i + j == 0 && i > 0) { // top-right corner
    worse_up = true; worse_right = true;
  }
  else if (i + j == 0 && i < 0) { // bottom-left corner
    worse_down = true; worse_left = true;
  }
  else if (i + j < 0 && i > j) { // top side
    worse_up = true;
  }
  else if (i + j > 0 && i < j) { // down side
    worse_down = true;
  }
  else if (i + j < 0 && i < j) { // left side
    worse_left = true;
  }
  else if (i + j > 0 && i > j) { // right side
    worse_right = true;
  }

  x1 = center_x + x1;
  y1 = center_x + y1;
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
    quad.distance = std::max(abs(quad.x - center_x), abs(quad.y - center_y));

    int n = pow(2, quad.distance);
    n = (quad.distance < 6) ? n : 32;
    for (int x2 = 0; x2 < QUAD_SIZE/n; x2++) {
      for (int y2 = 0; y2 < QUAD_SIZE/n; y2++) {
        float x = x1 * QUAD_SIZE * TILE_SIZE + x2 * TILE_SIZE * n;
        float y = y1 * QUAD_SIZE * TILE_SIZE + y2 * TILE_SIZE * n;

        glm::vec3 v_1 = glm::vec3(x + 0 * TILE_SIZE, GetHeight(x + 0 * TILE_SIZE, y + 0 * TILE_SIZE), y + 0 * TILE_SIZE);
        glm::vec3 v_2 = glm::vec3(x + 0 * TILE_SIZE, GetHeight(x + 0 * TILE_SIZE, y + n * TILE_SIZE), y + n * TILE_SIZE);
        glm::vec3 v_3 = glm::vec3(x + n * TILE_SIZE, GetHeight(x + n * TILE_SIZE, y + n * TILE_SIZE), y + n * TILE_SIZE);
        glm::vec3 v_4 = glm::vec3(x + n * TILE_SIZE, GetHeight(x + n * TILE_SIZE, y + 0 * TILE_SIZE), y + 0 * TILE_SIZE);

        glm::vec3 a = v_2 - v_1;
        glm::vec3 b = v_3 - v_1;
        glm::vec3 normal = glm::cross(a, b);

        int i = 4 * (x2 * (QUAD_SIZE/n) + y2);
        unsigned int v1 = AddVertexToQuad(&quad, i + 0, v_1, x2 * n,     y2 * n,     normal);
        unsigned int v2 = AddVertexToQuad(&quad, i + 1, v_2, x2 * n,     y2 * n + n, normal);
        unsigned int v3 = AddVertexToQuad(&quad, i + 2, v_3, x2 * n + n, y2 * n + n, normal);
        unsigned int v4 = AddVertexToQuad(&quad, i + 3, v_4, x2 * n + n, y2 * n,     normal);
      }
    }

    // Inside quads.
    int counter = 0;
    for (int x2 = 1; x2 < QUAD_SIZE/n - 1; x2++) {
      for (int y2 = 1; y2 < QUAD_SIZE/n - 1; y2++) {
        int i = 4 * (x2 * (QUAD_SIZE/n) + y2);
        // int j = 6 * (x2 * (QUAD_SIZE/n) + y2);
        quad.actual_indices[counter]   = i + 0;
        quad.actual_indices[counter+1] = i + 1;
        quad.actual_indices[counter+2] = i + 2;
        quad.actual_indices[counter+3] = i + 0;
        quad.actual_indices[counter+4] = i + 2;
        quad.actual_indices[counter+5] = i + 3;
        counter += 6;
      }
    }

    // Up side.
    for (int x2 = 2; x2 < QUAD_SIZE/n - 2; x2++) {
      int y2 = 0;
      if (worse_up) {
        if (x2 % 2 == 0) {
          quad.actual_indices[counter]   = 4 * (x2 * (QUAD_SIZE/n) + y2) + 0;
          quad.actual_indices[counter+1] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 1;
          quad.actual_indices[counter+2] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 2;
          quad.actual_indices[counter+3] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 0;
          quad.actual_indices[counter+4] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 2;
          quad.actual_indices[counter+5] = 4 * ((x2 + 1) * (QUAD_SIZE/n) + y2) + 3;
          counter += 6;
        } else {
          quad.actual_indices[counter]   = 4 * (x2 * (QUAD_SIZE/n) + y2) + 1;
          quad.actual_indices[counter+1] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 2;
          quad.actual_indices[counter+2] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 3;
          counter += 3;
        }
      } else {
        int i = 4 * (x2 * (QUAD_SIZE/n) + y2);
        quad.actual_indices[counter]   = i + 0;
        quad.actual_indices[counter+1] = i + 1;
        quad.actual_indices[counter+2] = i + 2;
        quad.actual_indices[counter+3] = i + 0;
        quad.actual_indices[counter+4] = i + 2;
        quad.actual_indices[counter+5] = i + 3;
        counter += 6;
      }
    }

    // Down side.
    for (int x2 = 2; x2 < QUAD_SIZE/n - 2; x2++) {
      int y2 = QUAD_SIZE/n - 1;
      if (worse_down) {
        if (x2 % 2 == 0) {
          quad.actual_indices[counter]   = 4 * (x2 * (QUAD_SIZE/n) + y2) + 0;
          quad.actual_indices[counter+1] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 1;
          quad.actual_indices[counter+2] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 3;
          quad.actual_indices[counter+3] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 1;
          quad.actual_indices[counter+4] = 4 * ((x2 + 1) * (QUAD_SIZE/n) + y2) + 2;
          quad.actual_indices[counter+5] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 3;
          counter += 6;
        } else {
          quad.actual_indices[counter]   = 4 * (x2 * (QUAD_SIZE/n) + y2) + 0;
          quad.actual_indices[counter+1] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 2;
          quad.actual_indices[counter+2] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 3;
          counter += 3;
        }
      } else {
        int i = 4 * (x2 * (QUAD_SIZE/n) + y2);
        quad.actual_indices[counter]   = i + 0;
        quad.actual_indices[counter+1] = i + 1;
        quad.actual_indices[counter+2] = i + 2;
        quad.actual_indices[counter+3] = i + 0;
        quad.actual_indices[counter+4] = i + 2;
        quad.actual_indices[counter+5] = i + 3;
        counter += 6;
      }
    }

    // Left side.
    for (int y2 = 2; y2 < QUAD_SIZE/n - 2; y2++) {
      int x2 = 0;
      if (worse_left) {
        if (y2 % 2 == 0) {
          quad.actual_indices[counter]   = 4 * (x2 * (QUAD_SIZE/n) + y2) + 0;
          quad.actual_indices[counter+1] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 2;
          quad.actual_indices[counter+2] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 3;
          quad.actual_indices[counter+3] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 0;
          quad.actual_indices[counter+4] = 4 * (x2 * (QUAD_SIZE/n) + y2 + 1) + 1;
          quad.actual_indices[counter+5] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 2;
          counter += 6;
        } else {
          quad.actual_indices[counter]   = 4 * (x2 * (QUAD_SIZE/n) + y2) + 1;
          quad.actual_indices[counter+1] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 2;
          quad.actual_indices[counter+2] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 3;
          counter += 3;
        }
      } else {
        int i = 4 * (x2 * (QUAD_SIZE/n) + y2);
        quad.actual_indices[counter]   = i + 0;
        quad.actual_indices[counter+1] = i + 1;
        quad.actual_indices[counter+2] = i + 2;
        quad.actual_indices[counter+3] = i + 0;
        quad.actual_indices[counter+4] = i + 2;
        quad.actual_indices[counter+5] = i + 3;
        counter += 6;
      }
    }

    // Right side.
    for (int y2 = 2; y2 < QUAD_SIZE/n - 2; y2++) {
      int x2 = QUAD_SIZE/n - 1;
      if (worse_right) {
        if (y2 % 2 == 0) {
          quad.actual_indices[counter]   = 4 * (x2 * (QUAD_SIZE/n) + y2) + 0;
          quad.actual_indices[counter+1] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 1;
          quad.actual_indices[counter+2] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 3;
          quad.actual_indices[counter+3] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 1;
          quad.actual_indices[counter+4] = 4 * (x2 * (QUAD_SIZE/n) + y2 + 1) + 2;
          quad.actual_indices[counter+5] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 3;
          counter += 6;
        } else {
          quad.actual_indices[counter]   = 4 * (x2 * (QUAD_SIZE/n) + y2) + 0;
          quad.actual_indices[counter+1] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 1;
          quad.actual_indices[counter+2] = 4 * (x2 * (QUAD_SIZE/n) + y2) + 2;
          counter += 3;
        }
      } else {
        int i = 4 * (x2 * (QUAD_SIZE/n) + y2);
        quad.actual_indices[counter]   = i + 0;
        quad.actual_indices[counter+1] = i + 1;
        quad.actual_indices[counter+2] = i + 2;
        quad.actual_indices[counter+3] = i + 0;
        quad.actual_indices[counter+4] = i + 2;
        quad.actual_indices[counter+5] = i + 3;
        counter += 6;
      }
    }

    // Top left corner.
    if (worse_up || worse_left) {
      if (worse_up && worse_left) {
        quad.actual_indices[counter]   = 4 * (0 * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+1] = 4 * (0 * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+2] = 4 * (1 * (QUAD_SIZE/n) + 0) + 3;
        quad.actual_indices[counter+3] = 4 * (0 * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+4] = 4 * (0 * (QUAD_SIZE/n) + 1) + 1;
        quad.actual_indices[counter+5] = 4 * (0 * (QUAD_SIZE/n) + 0) + 2;

        quad.actual_indices[counter+6]  = 4 * (0 * (QUAD_SIZE/n) + 1) + 1;
        quad.actual_indices[counter+7]  = 4 * (0 * (QUAD_SIZE/n) + 1) + 2;
        quad.actual_indices[counter+8]  = 4 * (0 * (QUAD_SIZE/n) + 1) + 3;
        quad.actual_indices[counter+9]  = 4 * (1 * (QUAD_SIZE/n) + 0) + 1;
        quad.actual_indices[counter+10] = 4 * (1 * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+11] = 4 * (1 * (QUAD_SIZE/n) + 0) + 3;
        counter += 12;
      } else if (worse_up) {
        quad.actual_indices[counter]   = 4 * (0 * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+1] = 4 * (0 * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+2] = 4 * (1 * (QUAD_SIZE/n) + 0) + 3;
        quad.actual_indices[counter+3]  = 4 * (1 * (QUAD_SIZE/n) + 0) + 1;
        quad.actual_indices[counter+4]  = 4 * (1 * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+5]  = 4 * (1 * (QUAD_SIZE/n) + 0) + 3;
        quad.actual_indices[counter+6]  = 4 * (0 * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+7] = 4 * (0 * (QUAD_SIZE/n) + 0) + 1;
        quad.actual_indices[counter+8] = 4 * (0 * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+9]  = 4 * (0 * (QUAD_SIZE/n) + 1) + 0;
        quad.actual_indices[counter+10]  = 4 * (0 * (QUAD_SIZE/n) + 1) + 1;
        quad.actual_indices[counter+11]  = 4 * (0 * (QUAD_SIZE/n) + 1) + 2;
        quad.actual_indices[counter+12]  = 4 * (0 * (QUAD_SIZE/n) + 1) + 0;
        quad.actual_indices[counter+13]  = 4 * (0 * (QUAD_SIZE/n) + 1) + 2;
        quad.actual_indices[counter+14]  = 4 * (0 * (QUAD_SIZE/n) + 1) + 3;
        counter += 15;
      } else {
        quad.actual_indices[counter] = 4 * (0 * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+1] = 4 * (0 * (QUAD_SIZE/n) + 1) + 1;
        quad.actual_indices[counter+2] = 4 * (0 * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+3] = 4 * (0 * (QUAD_SIZE/n) + 1) + 1;
        quad.actual_indices[counter+4] = 4 * (0 * (QUAD_SIZE/n) + 1) + 2;
        quad.actual_indices[counter+5] = 4 * (0 * (QUAD_SIZE/n) + 1) + 3;
        quad.actual_indices[counter+6] = 4 * (0 * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+7] = 4 * (0 * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+8] = 4 * (0 * (QUAD_SIZE/n) + 0) + 3;
        quad.actual_indices[counter+9]  = 4 * (1 * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+10] = 4 * (1 * (QUAD_SIZE/n) + 0) + 1;
        quad.actual_indices[counter+11] = 4 * (1 * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+12] = 4 * (1 * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+13] = 4 * (1 * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+14] = 4 * (1 * (QUAD_SIZE/n) + 0) + 3;
        counter += 15;
      }
    } else {
      quad.actual_indices[counter]   = 0;
      quad.actual_indices[counter+1] = 1;
      quad.actual_indices[counter+2] = 2;
      quad.actual_indices[counter+3] = 0;
      quad.actual_indices[counter+4] = 2;
      quad.actual_indices[counter+5] = 3;

      quad.actual_indices[counter+6]   = 4 * (1 * (QUAD_SIZE/n) + 0) + 0;
      quad.actual_indices[counter+7] = 4 * (1 * (QUAD_SIZE/n) + 0) + 1;
      quad.actual_indices[counter+8] = 4 * (1 * (QUAD_SIZE/n) + 0) + 2;
      quad.actual_indices[counter+9] = 4 * (1 * (QUAD_SIZE/n) + 0) + 0;
      quad.actual_indices[counter+10] = 4 * (1 * (QUAD_SIZE/n) + 0) + 2;
      quad.actual_indices[counter+11] = 4 * (1 * (QUAD_SIZE/n) + 0) + 3;

      quad.actual_indices[counter+12]   = 4 * (0 * (QUAD_SIZE/n) + 1) + 0;
      quad.actual_indices[counter+13] = 4 * (0 * (QUAD_SIZE/n) + 1) + 1;
      quad.actual_indices[counter+14] = 4 * (0 * (QUAD_SIZE/n) + 1) + 2;
      quad.actual_indices[counter+15] = 4 * (0 * (QUAD_SIZE/n) + 1) + 0;
      quad.actual_indices[counter+16] = 4 * (0 * (QUAD_SIZE/n) + 1) + 2;
      quad.actual_indices[counter+17] = 4 * (0 * (QUAD_SIZE/n) + 1) + 3;

      counter += 18;
    }

    // Top right corner.
    if (worse_up || worse_right) {
      if (worse_up && worse_right) {
        quad.actual_indices[counter]   = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+1] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 1;
        quad.actual_indices[counter+2] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 3;
        quad.actual_indices[counter+3] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 1;
        quad.actual_indices[counter+4] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 2;
        quad.actual_indices[counter+5] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 3;

        quad.actual_indices[counter+6 ] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+7 ] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 1;
        quad.actual_indices[counter+8 ] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+9 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 0;
        quad.actual_indices[counter+10] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 1;
        quad.actual_indices[counter+11] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 2;

        counter += 12;
      } else if (worse_up) {
        quad.actual_indices[counter]    = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+1]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 1;
        quad.actual_indices[counter+2]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 3;
        quad.actual_indices[counter+3]  = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+4]  = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 1;
        quad.actual_indices[counter+5]  = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+6]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 1;
        quad.actual_indices[counter+7]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+8]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 3;
        quad.actual_indices[counter+9]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 0;
        quad.actual_indices[counter+10] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 1;
        quad.actual_indices[counter+11] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 2;
        quad.actual_indices[counter+12] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 0;
        quad.actual_indices[counter+13] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 2;
        quad.actual_indices[counter+14] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 3;
        counter += 15;
      } else {
        quad.actual_indices[counter]    = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 1;
        quad.actual_indices[counter+1]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 2;
        quad.actual_indices[counter+2]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 3;
        quad.actual_indices[counter+3]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 0;
        quad.actual_indices[counter+4]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 1;
        quad.actual_indices[counter+5]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 2;
        quad.actual_indices[counter+6]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+7]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 1;
        quad.actual_indices[counter+8]  = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 3;
        quad.actual_indices[counter+9]  = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+10] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 1;
        quad.actual_indices[counter+11] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+12] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 0;
        quad.actual_indices[counter+13] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 2;
        quad.actual_indices[counter+14] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 3;
        counter += 15;
      }
    } else {
      quad.actual_indices[counter]   = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 0;
      quad.actual_indices[counter+1] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 1;
      quad.actual_indices[counter+2] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 2;
      quad.actual_indices[counter+3] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 0;
      quad.actual_indices[counter+4] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 2;
      quad.actual_indices[counter+5] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 0) + 3;

      quad.actual_indices[counter+6 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 0;
      quad.actual_indices[counter+7 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 1;
      quad.actual_indices[counter+8 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 2;
      quad.actual_indices[counter+9 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 0;
      quad.actual_indices[counter+10] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 2;
      quad.actual_indices[counter+11] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + 1) + 3;

      quad.actual_indices[counter+12] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 0;
      quad.actual_indices[counter+13] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 1;
      quad.actual_indices[counter+14] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 2;
      quad.actual_indices[counter+15] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 0;
      quad.actual_indices[counter+16] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 2;
      quad.actual_indices[counter+17] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + 0) + 3;

      counter += 18;
    }

    // Bottom left corner.
    if (worse_down || worse_left) {
      if (worse_down && worse_left) {
        quad.actual_indices[counter]   = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
        quad.actual_indices[counter+1] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
        quad.actual_indices[counter+2] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;
        quad.actual_indices[counter+3] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
        quad.actual_indices[counter+4] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+5] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;

        quad.actual_indices[counter+6 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
        quad.actual_indices[counter+7 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 2;
        quad.actual_indices[counter+8 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 3;
        quad.actual_indices[counter+9 ] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+10] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+11] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;

        counter += 12;
      } else if (worse_down) {
        quad.actual_indices[counter   ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
        quad.actual_indices[counter+1 ] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+2 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;
        quad.actual_indices[counter+3 ] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+4 ] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+5 ] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;
        quad.actual_indices[counter+6 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+7 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
        quad.actual_indices[counter+8 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;
        quad.actual_indices[counter+9 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
        quad.actual_indices[counter+10] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 1;
        quad.actual_indices[counter+11] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 2;
        quad.actual_indices[counter+12] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
        quad.actual_indices[counter+13] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 2;
        quad.actual_indices[counter+14] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 3;
        counter += 15;
      } else {
        quad.actual_indices[counter   ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
        quad.actual_indices[counter+1 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
        quad.actual_indices[counter+2 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;
        quad.actual_indices[counter+3 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
        quad.actual_indices[counter+4 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 2;
        quad.actual_indices[counter+5 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 3;
        quad.actual_indices[counter+6 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
        quad.actual_indices[counter+7 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+8 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;
        quad.actual_indices[counter+9 ] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+10] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
        quad.actual_indices[counter+11] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+12] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+13] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+14] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;
        counter += 15;
      }
    } else {
      quad.actual_indices[counter   ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
      quad.actual_indices[counter+1 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
      quad.actual_indices[counter+2 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
      quad.actual_indices[counter+3 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
      quad.actual_indices[counter+4 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
      quad.actual_indices[counter+5 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;
                                                                                 
      quad.actual_indices[counter+6 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
      quad.actual_indices[counter+7 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 1;
      quad.actual_indices[counter+8 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 2;
      quad.actual_indices[counter+9 ] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
      quad.actual_indices[counter+10] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 2;
      quad.actual_indices[counter+11] = 4 * (0 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 3;
                                                                                 
      quad.actual_indices[counter+12] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
      quad.actual_indices[counter+13] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
      quad.actual_indices[counter+14] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
      quad.actual_indices[counter+15] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
      quad.actual_indices[counter+16] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
      quad.actual_indices[counter+17] = 4 * (1 * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;

      counter += 18;
    }

    // Bottom right corner.
    if (worse_down || worse_right) {
      if (worse_down && worse_right) {
        quad.actual_indices[counter]   = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+1] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
        quad.actual_indices[counter+2] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+3] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+4] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+5] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 3;

        quad.actual_indices[counter+6 ] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+7 ] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
        quad.actual_indices[counter+8 ] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;
        quad.actual_indices[counter+9 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
        quad.actual_indices[counter+10] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 1;
        quad.actual_indices[counter+11] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 3;
        counter += 12;
      } else if (worse_down) {
        quad.actual_indices[counter   ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+1 ] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
        quad.actual_indices[counter+2 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+3 ] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+4 ] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
        quad.actual_indices[counter+5 ] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;
        quad.actual_indices[counter+6 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+7 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+8 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;
        quad.actual_indices[counter+9 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
        quad.actual_indices[counter+10] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 1;
        quad.actual_indices[counter+11] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 2;
        quad.actual_indices[counter+12] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
        quad.actual_indices[counter+13] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 2;
        quad.actual_indices[counter+14] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 3;
        counter += 15;
      } else {
        quad.actual_indices[counter   ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+1 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+2 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 3;
        quad.actual_indices[counter+3 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
        quad.actual_indices[counter+4 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 1;
        quad.actual_indices[counter+5 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 3;
        quad.actual_indices[counter+6 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+7 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
        quad.actual_indices[counter+8 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+9 ] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+10] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
        quad.actual_indices[counter+11] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+12] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
        quad.actual_indices[counter+13] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
        quad.actual_indices[counter+14] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;
        counter += 15;
      }
    } else {
      quad.actual_indices[counter   ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
      quad.actual_indices[counter+1 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
      quad.actual_indices[counter+2 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
      quad.actual_indices[counter+3 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
      quad.actual_indices[counter+4 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
      quad.actual_indices[counter+5 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;

      quad.actual_indices[counter+6 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
      quad.actual_indices[counter+7 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 1;
      quad.actual_indices[counter+8 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 2;
      quad.actual_indices[counter+9 ] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 0;
      quad.actual_indices[counter+10] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 2;
      quad.actual_indices[counter+11] = 4 * ((QUAD_SIZE/n - 1) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 2)) + 3;

      quad.actual_indices[counter+12] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
      quad.actual_indices[counter+13] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 1;
      quad.actual_indices[counter+14] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
      quad.actual_indices[counter+15] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 0;
      quad.actual_indices[counter+16] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 2;
      quad.actual_indices[counter+17] = 4 * ((QUAD_SIZE/n - 2) * (QUAD_SIZE/n) + (QUAD_SIZE/n - 1)) + 3;

      counter += 18;
    }

    quad.buffer_size = QUAD_SIZE * QUAD_SIZE * 4 / (n * n);
    // quad.lod_size = QUAD_SIZE * QUAD_SIZE * 6 / (n * n);
    quad.lod_size = counter;

    if (!quad.initialized) {
      glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
      glBufferData(GL_ARRAY_BUFFER, quad.buffer_size * sizeof(glm::vec3), &quad.indexed_vertices[0], GL_DYNAMIC_DRAW);
      
      glBindBuffer(GL_ARRAY_BUFFER, quad.uv_buffer);
      glBufferData(GL_ARRAY_BUFFER, quad.buffer_size * sizeof(glm::vec2), &quad.indexed_uvs[0], GL_DYNAMIC_DRAW);
      
      glBindBuffer(GL_ARRAY_BUFFER, quad.normal_buffer);
      glBufferData(GL_ARRAY_BUFFER, quad.buffer_size * sizeof(glm::vec3), &quad.indexed_normals[0], GL_DYNAMIC_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, quad.tangent_buffer);
      glBufferData(GL_ARRAY_BUFFER, quad.buffer_size * sizeof(glm::vec3), &quad.indexed_tangents[0], GL_DYNAMIC_DRAW);
      
      glBindBuffer(GL_ARRAY_BUFFER, quad.bitangent_buffer);
      glBufferData(GL_ARRAY_BUFFER, quad.buffer_size * sizeof(glm::vec3), &quad.indexed_bitangents[0], GL_DYNAMIC_DRAW);
      
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.element_buffer);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, quad.lod_size * sizeof(unsigned int), &quad.actual_indices[0], GL_DYNAMIC_DRAW);
      quad.initialized = true;
    } else {
      glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
      glBufferSubData(GL_ARRAY_BUFFER, 0, quad.buffer_size * sizeof(glm::vec3), &quad.indexed_vertices[0]);
      
      glBindBuffer(GL_ARRAY_BUFFER, quad.uv_buffer);
      glBufferSubData(GL_ARRAY_BUFFER, 0, quad.buffer_size * sizeof(glm::vec2), &quad.indexed_uvs[0]);
      
      glBindBuffer(GL_ARRAY_BUFFER, quad.normal_buffer);
      glBufferSubData(GL_ARRAY_BUFFER, 0, quad.buffer_size * sizeof(glm::vec3), &quad.indexed_normals[0]);

      glBindBuffer(GL_ARRAY_BUFFER, quad.tangent_buffer);
      glBufferSubData(GL_ARRAY_BUFFER, 0, quad.buffer_size * sizeof(glm::vec3), &quad.indexed_tangents[0]);
      
      glBindBuffer(GL_ARRAY_BUFFER, quad.bitangent_buffer);
      glBufferSubData(GL_ARRAY_BUFFER, 0, quad.buffer_size * sizeof(glm::vec3), &quad.indexed_bitangents[0]);
      
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.element_buffer);
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, quad.lod_size * sizeof(unsigned int), &quad.actual_indices[0]);
    }
    break;
  }
}

void Terrain::UpdateQuads() {
  int center_x = player_->position().x / (QUAD_SIZE * TILE_SIZE);
  int center_y = player_->position().z / (QUAD_SIZE * TILE_SIZE);

  if (center_x == last_center_x_ && center_y == last_center_y_) return;

  last_center_x_ = center_x;
  last_center_y_ = center_y;

  // Delete distant quads.
  for (int i = 0; i < NUM_QUADS; i++) {
    TerrainQuad& quad = quads_[i];

    quad.empty = true;
    // int max_distance = BIG_QUAD_SIDE / 2;
    // if (
    //   abs(quad.x - center_x) > max_distance || 
    //   abs(quad.y - center_y) > max_distance
    // ) {
    //   quad.empty = true;
    // }
  }

  for (int i = -BIG_QUAD_SIDE / 2; i <= BIG_QUAD_SIDE / 2; i++) {
    for (int j = -BIG_QUAD_SIDE / 2; j <= BIG_QUAD_SIDE / 2; j++) {
      int x = center_x + i;
      int y = center_y + j;
      UpdateQuad(
        i, j, center_x, center_y
      );
    }
  }
}

void Terrain::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
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

  // Textures.
  shader_.BindTexture("DiffuseTextureSampler", diffuse_texture_id_);
  shader_.BindTexture("NormalTextureSampler", normal_texture_id_);
  shader_.BindTexture("SpecularTextureSampler", specular_texture_id_);
  shader_.BindTexture("HeightMapSampler", height_map_);

  shader_.BindBuffer(tile_vertex_buffers_[0], 0, 3);
  shader_.BindBuffer(tile_uv_buffers_[0], 1, 2);
  shader_.BindBuffer(tile_normal_buffers_[0], 2, 3);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tile_index_buffers_[0]);

  int player_pos_x = player_->position().x / 32 - 128;
  int player_pos_z = player_->position().z / 32 - 128;
  glUniform2i(shader_.GetUniformId("PlayerPosition"), player_pos_x, player_pos_z);

  for (int i = 0; i < 257; i++) {
    for (int j = 0; j < 257; j++) {
      data[i*257 + j] = (GetHeight((player_pos_x + j)*32, (player_pos_z + i)*32));
    }
  }

  glBindTexture(GL_TEXTURE_2D, height_map_);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 257, 257, GL_RED, GL_FLOAT, data);
  glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*) 0, 256 * 256);

  // for (int i = 0; i < NUM_QUADS; i++) {
  //   TerrainQuad& quad = quads_[i];

  //   // Buffers.
  //   shader_.BindBuffer(quad.vertex_buffer, 0, 3);
  //   shader_.BindBuffer(quad.uv_buffer, 1, 2);
  //   shader_.BindBuffer(quad.normal_buffer, 2, 3);
  //   
  //   // Index buffer.
  //   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.element_buffer);

  //   // glDrawElements(GL_TRIANGLES, quad.indices.size(), GL_UNSIGNED_INT, (void*) 0);
  //   glDrawElements(GL_TRIANGLES, quad.lod_size, GL_UNSIGNED_INT, (void*) 0);
  // }

  // glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei primcount);
  shader_.Clear();
}

void Terrain::Clean() {
  // glDeleteBuffers(1, &vertex_buffer_);
  // glDeleteBuffers(1, &uv_buffer_);
  // glDeleteBuffers(1, &normal_buffer_);
  // glDeleteBuffers(1, &element_buffer_);
  // glDeleteBuffers(1, &tangent_buffer_);
  // glDeleteBuffers(1, &bitangent_buffer_);
}

} // End of namespace.
