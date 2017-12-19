#include "mesh.hpp"

namespace Sibyl {

void Mesh::ComputeTangentBasis() {
  for (unsigned int i = 0; i < vertices_.size(); i += 3) {
    // Shortcuts for vertices
    glm::vec3& v0 = vertices_[i+0];
    glm::vec3& v1 = vertices_[i+1];
    glm::vec3& v2 = vertices_[i+2];
    
    // Shortcuts for UVs
    glm::vec2& uv0 = uvs_[i+0];
    glm::vec2& uv1 = uvs_[i+1];
    glm::vec2& uv2 = uvs_[i+2];
    
    // Edges of the triangle : postion delta
    glm::vec3 deltaPos1 = v1-v0;
    glm::vec3 deltaPos2 = v2-v0;
    
    // UV delta
    glm::vec2 deltaUV1 = uv1-uv0;
    glm::vec2 deltaUV2 = uv2-uv0;
    
    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    glm::vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;
    
    // Set the same tangent for all three vertices of the triangle.
    // They will be merged later, in vboindexer.cpp
    tangents_.push_back(tangent);
    tangents_.push_back(tangent);
    tangents_.push_back(tangent);
    
    // Same thing for binormals
    bitangents_.push_back(bitangent);
    bitangents_.push_back(bitangent);
    bitangents_.push_back(bitangent);
  }
  
  // See "Going Further"
  for (unsigned int i = 0; i < vertices_.size(); i++) {
    glm::vec3& n = normals_[i];
    glm::vec3& t = tangents_[i];
    glm::vec3& b = bitangents_[i];
    
    // Gram-Schmidt orthogonalize
    t = glm::normalize(t - n * glm::dot(n, t));
    
    // Calculate handedness
    if (glm::dot(glm::cross(n, t), b) < 0.0f){
      t = t * -1.0f;
    }
  }
}

// Returns true iif v1 can be considered equal to v2
bool Mesh::IsNear(float v1, float v2) {
  return fabs(v1 - v2) < 0.01f;
}

// Searches through all already-exported vertices
// for a similar one.
// Similar = same position + same UVs + same normal
bool Mesh::GetSimilarVertexIndex( 
  unsigned short i,
  unsigned short& result
) {
  glm::vec3& in_vertex = vertices_[i];
  glm::vec2& in_uv     = uvs_[i];
  glm::vec3& in_normal = normals_[i];

  // Lame linear search.
  for (unsigned int i = 0; i < indexed_vertices_.size(); i++) {
    if (
      IsNear(in_vertex.x , indexed_vertices_[i].x) &&
      IsNear(in_vertex.y , indexed_vertices_[i].y) &&
      IsNear(in_vertex.z , indexed_vertices_[i].z) &&
      IsNear(in_uv.x     , indexed_uvs_     [i].x) &&
      IsNear(in_uv.y     , indexed_uvs_     [i].y) &&
      IsNear(in_normal.x , indexed_normals_ [i].x) &&
      IsNear(in_normal.y , indexed_normals_ [i].y) &&
      IsNear(in_normal.z , indexed_normals_ [i].z)
    ) {
      result = i;
      return true;
    }
  }
  // No other vertex could be used instead.
  // Looks like we'll have to add it to the VBO.
  return false;
}

void Mesh::IndexVBO() {
  // For each input vertex
  for (unsigned int i = 0; i < vertices_.size(); i++) {
    // Try to find a similar vertex in out_XXXX
    unsigned short index;
    bool found = GetSimilarVertexIndex(i, index);
    
    if (found) { // A similar vertex is already in the VBO, use it instead !
      indices_.push_back(index);
      
      // Average the tangents and the bitangents.
      indexed_tangents_[index] += tangents_[i];
      indexed_bitangents_[index] += bitangents_[i];
    } else {
      // If not, it needs to be added in the output data.
      indexed_vertices_.push_back(vertices_[i]);
      indexed_uvs_.push_back(uvs_[i]);
      indexed_normals_.push_back(normals_[i]);
      indexed_tangents_.push_back(tangents_[i]);
      indexed_bitangents_.push_back(bitangents_[i]);
      indices_.push_back((unsigned short) indexed_vertices_.size() - 1);
    }
  }
}

void Mesh::CreateBuffers() {
  ComputeTangentBasis();
  IndexVBO();
  
  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_vertices_.size() * sizeof(glm::vec3), &indexed_vertices_[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &uv_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_uvs_.size() * sizeof(glm::vec2), &indexed_uvs_[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &normal_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_normals_.size() * sizeof(glm::vec3), &indexed_normals_[0], GL_STATIC_DRAW);

  glGenBuffers(1, &tangent_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, tangent_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_tangents_.size() * sizeof(glm::vec3), &indexed_tangents_[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &bitangent_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, bitangent_buffer_);
  glBufferData(GL_ARRAY_BUFFER, indexed_bitangents_.size() * sizeof(glm::vec3), &indexed_bitangents_[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned short), &indices_[0] , GL_STATIC_DRAW);
}

bool Mesh::LoadObj(const std::string& path) {
  printf("Loading OBJ file %s...\n", path.c_str());

  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<glm::vec3> temp_vertices; 
  std::vector<glm::vec2> temp_uvs;
  std::vector<glm::vec3> temp_normals;

  FILE * file = fopen(path.c_str(), "r");
  if (file == NULL) {
    printf("Impossible to open the .obj file!\n");
    getchar();
    return false;
  }

  while (1) {
    char lineHeader[128];
    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);
    if (res == EOF)
      break; // EOF = End Of File. Quit the loop.

    // else : parse lineHeader
    
    if (strcmp(lineHeader, "v") == 0) {
      glm::vec3 vertex;
      fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
      temp_vertices.push_back(vertex);
    } else if (strcmp( lineHeader, "vt") == 0 ) {
      glm::vec2 uv;
      fscanf(file, "%f %f\n", &uv.x, &uv.y );
      // uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
      temp_uvs.push_back(uv);
    } else if (strcmp( lineHeader, "vn" ) == 0) {
      glm::vec3 normal;
      fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
      temp_normals.push_back(normal);
    } else if (strcmp( lineHeader, "f" ) == 0) {
      std::string vertex1, vertex2, vertex3;
      unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
      int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
      if (matches != 9) {
        printf("File can't be read by our simple parser :-( Try exporting with other options\n");
        fclose(file);
        return false;
      }
      vertexIndices.push_back(vertexIndex[0]);
      vertexIndices.push_back(vertexIndex[1]);
      vertexIndices.push_back(vertexIndex[2]);
      uvIndices    .push_back(uvIndex[0]);
      uvIndices    .push_back(uvIndex[1]);
      uvIndices    .push_back(uvIndex[2]);
      normalIndices.push_back(normalIndex[0]);
      normalIndices.push_back(normalIndex[1]);
      normalIndices.push_back(normalIndex[2]);
    } else {
      // Probably a comment, eat up the rest of the line
      char stupidBuffer[1000];
      fgets(stupidBuffer, 1000, file);
    }
  }

  // For each vertex of each triangle
  for (unsigned int i = 0; i < vertexIndices.size(); i++) {
    // Get the indices of its attributes
    unsigned int vertexIndex = vertexIndices[i];
    unsigned int uvIndex = uvIndices[i];
    unsigned int normalIndex = normalIndices[i];
    
    // Get the attributes thanks to the index
    glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
    glm::vec2 uv = temp_uvs[ uvIndex-1 ];
    glm::vec3 normal = temp_normals[ normalIndex-1 ];
    
    // Put the attributes in buffers
    vertices_.push_back(vertex);
    uvs_     .push_back(uv);
    normals_ .push_back(normal);
  }
  fclose(file);
 
  CreateBuffers();
  return true;
}

void Mesh::Clean() {
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteBuffers(1, &uv_buffer_);
  glDeleteBuffers(1, &normal_buffer_);
  glDeleteBuffers(1, &element_buffer_);
  glDeleteBuffers(1, &tangent_buffer_);
  glDeleteBuffers(1, &bitangent_buffer_);
}

} // End of namespace.
