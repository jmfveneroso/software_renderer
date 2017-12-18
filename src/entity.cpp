#include "entity.hpp"

namespace Sibyl {

bool IEntity::LoadObj(
  const char * path, 
  std::vector<glm::vec3>& out_vertices, 
  std::vector<glm::vec2>& out_uvs,
  std::vector<glm::vec3>& out_normals
) {
  printf("Loading OBJ file %s...\n", path);

  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<glm::vec3> temp_vertices; 
  std::vector<glm::vec2> temp_uvs;
  std::vector<glm::vec3> temp_normals;

  FILE * file = fopen(path, "r");
  if (file == NULL) {
    printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
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
    
    if ( strcmp( lineHeader, "v" ) == 0 ){
      glm::vec3 vertex;
      fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
      temp_vertices.push_back(vertex);
    } else if ( strcmp( lineHeader, "vt" ) == 0 ){
      glm::vec2 uv;
      fscanf(file, "%f %f\n", &uv.x, &uv.y );
      // uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
      temp_uvs.push_back(uv);
    } else if ( strcmp( lineHeader, "vn" ) == 0 ){
      glm::vec3 normal;
      fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
      temp_normals.push_back(normal);
    } else if ( strcmp( lineHeader, "f" ) == 0 ){
      std::string vertex1, vertex2, vertex3;
      unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
      int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
      if (matches != 9){
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
  for (unsigned int i=0; i<vertexIndices.size(); i++) {
    // Get the indices of its attributes
    unsigned int vertexIndex = vertexIndices[i];
    unsigned int uvIndex = uvIndices[i];
    unsigned int normalIndex = normalIndices[i];
    
    // Get the attributes thanks to the index
    glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
    glm::vec2 uv = temp_uvs[ uvIndex-1 ];
    glm::vec3 normal = temp_normals[ normalIndex-1 ];
    
    // Put the attributes in buffers
    out_vertices.push_back(vertex);
    out_uvs     .push_back(uv);
    out_normals .push_back(normal);
  }
  fclose(file);
  return true;
}

void IEntity::ComputeTangentBasis(
  std::vector<glm::vec3> & vertices,
  std::vector<glm::vec2> & uvs,
  std::vector<glm::vec3> & normals,
  std::vector<glm::vec3> & tangents,
  std::vector<glm::vec3> & bitangents
) {
  for (unsigned int i = 0; i < vertices.size(); i += 3) {
    // Shortcuts for vertices
    glm::vec3& v0 = vertices[i+0];
    glm::vec3& v1 = vertices[i+1];
    glm::vec3& v2 = vertices[i+2];
    
    // Shortcuts for UVs
    glm::vec2& uv0 = uvs[i+0];
    glm::vec2& uv1 = uvs[i+1];
    glm::vec2& uv2 = uvs[i+2];
    
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
    tangents.push_back(tangent);
    tangents.push_back(tangent);
    tangents.push_back(tangent);
    
    // Same thing for binormals
    bitangents.push_back(bitangent);
    bitangents.push_back(bitangent);
    bitangents.push_back(bitangent);
  }
  
  // See "Going Further"
  for (unsigned int i = 0; i < vertices.size(); i++) {
    glm::vec3 & n = normals[i];
    glm::vec3 & t = tangents[i];
    glm::vec3 & b = bitangents[i];
    
    // Gram-Schmidt orthogonalize
    t = glm::normalize(t - n * glm::dot(n, t));
    
    // Calculate handedness
    if (glm::dot(glm::cross(n, t), b) < 0.0f){
      t = t * -1.0f;
    }
  }
}

// Returns true iif v1 can be considered equal to v2
bool IEntity::IsNear(float v1, float v2) {
  return fabs(v1 - v2) < 0.01f;
}

// Searches through all already-exported vertices
// for a similar one.
// Similar = same position + same UVs + same normal
bool IEntity::GetSimilarVertexIndex( 
  glm::vec3& in_vertex, 
  glm::vec2& in_uv, 
  glm::vec3& in_normal, 
  std::vector<glm::vec3>& out_vertices,
  std::vector<glm::vec2>& out_uvs,
  std::vector<glm::vec3>& out_normals,
  unsigned short& result
) {
  // Lame linear search.
  for (unsigned int i=0; i<out_vertices.size(); i++) {
    if (
      IsNear(in_vertex.x , out_vertices[i].x) &&
      IsNear(in_vertex.y , out_vertices[i].y) &&
      IsNear(in_vertex.z , out_vertices[i].z) &&
      IsNear(in_uv.x     , out_uvs     [i].x) &&
      IsNear(in_uv.y     , out_uvs     [i].y) &&
      IsNear(in_normal.x , out_normals [i].x) &&
      IsNear(in_normal.y , out_normals [i].y) &&
      IsNear(in_normal.z , out_normals [i].z)
    ) {
      result = i;
      return true;
    }
  }
  // No other vertex could be used instead.
  // Looks like we'll have to add it to the VBO.
  return false;
}

void IEntity::IndexVBO(
  std::vector<glm::vec3>& in_vertices,
  std::vector<glm::vec2>& in_uvs,
  std::vector<glm::vec3>& in_normals,
  std::vector<glm::vec3>& in_tangents,
  std::vector<glm::vec3>& in_bitangents,
  std::vector<unsigned short>& out_indices,
  std::vector<glm::vec3>& out_vertices,
  std::vector<glm::vec2>& out_uvs,
  std::vector<glm::vec3>& out_normals,
  std::vector<glm::vec3>& out_tangents,
  std::vector<glm::vec3>& out_bitangents
) {
  // For each input vertex
  for (unsigned int i = 0; i < in_vertices.size(); i++) {
    // Try to find a similar vertex in out_XXXX
    unsigned short index;
    bool found = GetSimilarVertexIndex(in_vertices[i], in_uvs[i], in_normals[i], out_vertices, out_uvs, out_normals, index);
    
    if (found) { // A similar vertex is already in the VBO, use it instead !
      out_indices.push_back( index );
      
      // Average the tangents and the bitangents.
      out_tangents[index] += in_tangents[i];
      out_bitangents[index] += in_bitangents[i];
    } else {
      // If not, it needs to be added in the output data.
      out_vertices.push_back( in_vertices[i]);
      out_uvs     .push_back( in_uvs[i]);
      out_normals .push_back( in_normals[i]);
      out_tangents .push_back( in_tangents[i]);
      out_bitangents .push_back( in_bitangents[i]);
      out_indices .push_back( (unsigned short)out_vertices.size() - 1 );
    }
  }
}

void IEntity::ComputeIndexedVertices(
  std::vector<glm::vec3>& vertices,
  std::vector<glm::vec2>& uvs,
  std::vector<glm::vec3>& normals,
  std::vector<unsigned short>& indices,
  GLuint* vertex_buffer,
  GLuint* uv_buffer,
  GLuint* normal_buffer,
  GLuint* tangent_buffer,
  GLuint* bitangent_buffer,
  GLuint* element_buffer
) {
  std::vector<glm::vec3> tangents;
  std::vector<glm::vec3> bitangents;
  ComputeTangentBasis(
    vertices, uvs, normals, // Input.
    tangents, bitangents    // Output.
  );

  std::vector<glm::vec3> indexed_vertices;
  std::vector<glm::vec2> indexed_uvs;
  std::vector<glm::vec3> indexed_normals;
  std::vector<glm::vec3> indexed_tangents;
  std::vector<glm::vec3> indexed_bitangents;
  IndexVBO(
    vertices, uvs, normals, tangents, bitangents, 
    indices, indexed_vertices, indexed_uvs, 
    indexed_normals, indexed_tangents, indexed_bitangents
  );
  
  glGenBuffers(1, vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, uv_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, *uv_buffer);
  glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, normal_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, *normal_buffer);
  glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

  glGenBuffers(1, tangent_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, *tangent_buffer);
  glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, bitangent_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, *bitangent_buffer);
  glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, element_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *element_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);
}

Solid::Solid(
  const std::string& filename, 
  Shader shader, 
  GLuint diffuse_texture_id, 
  GLuint normal_texture_id, 
  GLuint specular_texture_id
) : shader_(shader),
    diffuse_texture_id_(diffuse_texture_id), 
    normal_texture_id_(normal_texture_id), 
    specular_texture_id_(specular_texture_id),
    position_(glm::vec3(0.0, 0.0, 0.0)) {
  LoadModel(filename);
}

void Solid::LoadModel(const std::string& filename) {
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals; 
  LoadObj(filename.c_str(), vertices_, uvs, normals);
  ComputeIndexedVertices(
    vertices_, uvs, normals, indices_,
    &vertex_buffer_,
    &uv_buffer_,
    &normal_buffer_,
    &tangent_buffer_,
    &bitangent_buffer_,
    &element_buffer_
  );
}

void Solid::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
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
  
  // Index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, (void*) 0);

  shader_.Clear();
}

void Solid::Clean() {
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteBuffers(1, &uv_buffer_);
  glDeleteBuffers(1, &normal_buffer_);
  glDeleteBuffers(1, &element_buffer_);
  glDeleteBuffers(1, &tangent_buffer_);
  glDeleteBuffers(1, &bitangent_buffer_);
}

float Water::move_factor = 0;

Water::Water(
  const std::string& filename, 
  Shader shader, 
  GLuint diffuse_texture_id, 
  GLuint normal_texture_id, 
  GLuint reflection_texture_id,
  GLuint refraction_texture_id,
  GLuint refraction_depth_texture_id
) : Solid(filename, shader, diffuse_texture_id, normal_texture_id, 0),
    reflection_texture_id_(reflection_texture_id),
    refraction_texture_id_(refraction_texture_id),
    refraction_depth_texture_id_(refraction_depth_texture_id) {
  LoadModel(filename);
}

void Water::UpdateMoveFactor(float seconds) {
  move_factor += WAVE_SPEED * seconds;
  move_factor = fmod(move_factor, 1);
}

void Water::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
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

  glUniform3fv(shader_.GetUniformId("cameraPosition"), 1, (float*) &camera);
  glUniform1f(shader_.GetUniformId("moveFactor"), Water::move_factor);

  // Textures.
  shader_.BindTexture("ReflectionTextureSampler", reflection_texture_id_);
  shader_.BindTexture("RefractionTextureSampler", refraction_texture_id_);
  shader_.BindTexture("dudvMap", diffuse_texture_id_);
  shader_.BindTexture("normalMap", normal_texture_id_);
  shader_.BindTexture("depthMap", refraction_depth_texture_id_);

  // Buffers.
  shader_.BindBuffer(vertex_buffer_, 0, 3);
  shader_.BindBuffer(uv_buffer_, 1, 2);
  shader_.BindBuffer(normal_buffer_, 2, 3);
  shader_.BindBuffer(tangent_buffer_, 3, 3);
  shader_.BindBuffer(bitangent_buffer_, 4, 3);
  
  // Draw.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT, (void*)0);

  shader_.Clear();
}

Terrain::Terrain(
  Shader shader, 
  GLuint diffuse_texture_id, 
  GLuint normal_texture_id, 
  GLuint specular_texture_id
) : shader_(shader),
    diffuse_texture_id_(diffuse_texture_id), 
    normal_texture_id_(normal_texture_id), 
    specular_texture_id_(specular_texture_id) {

  vertices_.push_back(glm::vec3(-1, 0, -1));
  uvs_.push_back(glm::vec2(0, 0));
  normals_.push_back(glm::vec3(0, 1, 0));

  vertices_.push_back(glm::vec3(-1, 0, 1));
  uvs_.push_back(glm::vec2(0, 1));
  normals_.push_back(glm::vec3(0, 1, 0));

  vertices_.push_back(glm::vec3(1, 0, 1));
  uvs_.push_back(glm::vec2(1, 1));
  normals_.push_back(glm::vec3(0, 1, 0));

  vertices_.push_back(glm::vec3(-1, 0, 1));
  uvs_.push_back(glm::vec2(0, 1));
  normals_.push_back(glm::vec3(0, 1, 0));

  vertices_.push_back(glm::vec3(1, 0, 1));
  uvs_.push_back(glm::vec2(1, 1));
  normals_.push_back(glm::vec3(0, 1, 0));

  vertices_.push_back(glm::vec3(1, 0, -1));
  uvs_.push_back(glm::vec2(1, 0));
  normals_.push_back(glm::vec3(0, 1, 0));
}

void Terrain::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
}

} // End of namespace.
