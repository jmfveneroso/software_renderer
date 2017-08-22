#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <stdio.h>
#include <vector>
// #include "vector.h"

// typedef struct {
//   int vertex_index;
//   int tex_coord_index;
//   int normal_index;
// } ObjIndex;
// 
// typedef struct {
//   Vector4f* positions;
//   Vector4f* tex_coords;
//   Vector4f* normals;
//   ObjIndex* indices;
//   size_t num_positions;
//   size_t num_tex_coords;
//   size_t num_normals;
//   size_t num_indices;
//   bool has_tex_coords;
//   bool has_normals;
// } ObjModel;
// 
// typedef struct Token {
//   char value[128];
//   size_t size;
//   Token* next;
// } Token;
// 
// Vector4f* AddVectorToArray(Vector4f* arr, size_t size, Vector4f v) {
//   arr = (Vector4f*) realloc(arr, sizeof(Vector4f) * (size + 1));
//   arr[size] = v;
//   return arr;
// }
// 
// void AddObjIndexToArray(ObjModel* model, Token* tk) { 
//   bool has_value[3];
//   int values[3];
// 
//   char* tkn_begin = tk->value;
//   char* tkn_end   = NULL;
//   for (int i = 0; i < 3; i++) {
//     tkn_end = tkn_begin;
//     int reached_end = 0;
//     while (*tkn_end != '/' && *tkn_end != '\0') tkn_end++;
//     if (*tkn_end == '\0') reached_end = 1;
//     *tkn_end = '\0';
//      
//     has_value[i] = tkn_begin[0] != '\0';
//     if (has_value[i]) values[i] = atoi(tkn_begin);
//     else values[i] = 0.0f;
//     tkn_begin = tkn_end + (1 - reached_end);
//   }
// 
//   ObjIndex obj_idx;
//   obj_idx.vertex_index = values[0] - 1;
// 
//   if (has_value[1]) { 
//     model->has_tex_coords = true;
//     obj_idx.tex_coord_index = values[1] - 1;
//   }
// 
//   if (has_value[2]) { 
//     model->has_normals = true;
//     obj_idx.normal_index = values[2] - 1;
//   }
// 
//   model->indices = (ObjIndex*) realloc(model->indices, sizeof(ObjIndex) * (model->num_indices + 1));
//   model->indices[model->num_indices] = obj_idx;
// }
// 
// bool ObjIndexEquals(ObjIndex* a, ObjIndex* b) {
//   return a->vertex_index    == b->vertex_index    &&
//          a->tex_coord_index == b->tex_coord_index &&
//          a->normal_index    == b->normal_index;
// }
// 
// bool ObjIndexHash(ObjIndex* obj) {
//   int base = 17;
//   int multiplier = 31;
// 
//   int result = base;
//   result = multiplier * result + obj->vertex_index;
//   result = multiplier * result + obj->tex_coord_index;
//   result = multiplier * result + obj->normal_index;
//   return result;
// }
// 
// #define STATE_UNKNOWN 0
// #define STATE_V       1
// #define STATE_VT      2
// #define STATE_VN      3
// #define STATE_F       4
// 
// size_t ReadLine(FILE* file, char* buffer) {
//   size_t num_read = 0;
//   char c;
//   while (fread(&c, sizeof(char), 1, file) == 1) {
//     if (c == '\n') break;
//     buffer[num_read] = c;
//     num_read++;
//   }
//   buffer[num_read] = '\0';
//   return num_read;
// }
// 
// Token* SplitTokens(char* buffer, size_t* num_tokens) {
//   Token* first_token = NULL;
//   Token* cur_token = NULL;
//   bool at_space = true;
//   size_t pos = 0;
//   (*num_tokens) = 0;
//   char* c;
//   for (c = buffer; *c != '\0'; c++) {
//     if (*c == ' ' || *c == '\t') {
//       if (at_space) continue;
//       Token* new_token = (Token*) malloc(sizeof(Token));
//       new_token->next = NULL;
//       cur_token->next = new_token;
//       cur_token->size = 0;
//       cur_token = new_token;
//       pos = 0;
//       at_space = true;
//       (*num_tokens)++;
//       continue;
//     } else if (first_token == NULL) {
//       first_token = (Token*) malloc(sizeof(Token));
//       first_token->next = NULL;
//       first_token->size = 0;
//       cur_token = first_token;
//       (*num_tokens)++;
//     }
// 
//     cur_token->value[pos++] = *c; 
//     cur_token->size++; 
//     at_space = false;
//   }
//   return first_token;
// }
// 
// void CalculateNormals(ObjModel* model) {
// 
// }
// 
// ObjModel* CreateObjModel(const char filename[]) {
//   ObjModel* obj = (ObjModel*) malloc(sizeof(ObjModel));
//   obj->positions = (Vector4f*) calloc(0, sizeof(Vector4f));
//   obj->tex_coords = (Vector4f*) calloc(0, sizeof(Vector4f));
//   obj->normals = (Vector4f*) calloc(0, sizeof(Vector4f));
//   obj->indices = (ObjIndex*) calloc(0, sizeof(ObjIndex));
//   obj->num_positions = 0;
//   obj->num_tex_coords = 0;
//   obj->num_normals = 0;
//   obj->num_indices = 0;
//   obj->indices = NULL;
//   obj->has_tex_coords = false;
//   obj->has_normals = false;
// 
//   FILE* file = fopen(filename, "rb");
//   if (!file) {
//     printf("Problem opening file %s.\n", filename);
//     return NULL;
//   }
// 
//   char buffer[128];
//   while (ReadLine(file, buffer) > 0) {
//     size_t num_tokens = 0;
//     Token* tkn = SplitTokens(buffer, &num_tokens);
//     if (num_tokens == 0) continue;
//     if (tkn->value[0] == '#') continue;
// 
//     printf("Line: %s num tks: %lu\n", buffer, num_tokens);
//     short state = STATE_UNKNOWN;
//     if      (strcmp(tkn->value, "v" ) == 0) state = STATE_V;
//     else if (strcmp(tkn->value, "vt") == 0) state = STATE_VT;
//     else if (strcmp(tkn->value, "vn") == 0) state = STATE_VN;
//     else if (strcmp(tkn->value, "f" ) == 0) state = STATE_F;
// 
//     tkn = tkn->next;
//     switch (state) {
//       case STATE_V: {
//         float x = atof(tkn->value); tkn = tkn->next;
//         float y = atof(tkn->value); tkn = tkn->next;
//         float z = atof(tkn->value); tkn = tkn->next;
//         obj->positions = AddVectorToArray(obj->positions, obj->num_positions, CreateVector4f(x, y, z, 1));
//         obj->num_positions++;
//         break;
//       }
//       case STATE_VT: {
//         float x = atof(tkn->value); tkn = tkn->next;
//         float y = 1.0f - atof(tkn->value); tkn = tkn->next;
//         obj->tex_coords = AddVectorToArray(obj->tex_coords, obj->num_tex_coords, CreateVector4f(x, y, 0, 0));
//         obj->num_tex_coords++;
//         break;
//       }
//       case STATE_VN: {
//         float x = atof(tkn->value); tkn = tkn->next;
//         float y = atof(tkn->value); tkn = tkn->next;
//         float z = atof(tkn->value); tkn = tkn->next;
//         obj->normals = AddVectorToArray(obj->normals, obj->num_normals, CreateVector4f(x, y, z, 0));
//         obj->num_normals++;
//         break;
//       }
//       case STATE_F: {
//         char value[128]; 
//         Token* first_tkn = tkn;
//         Token* second_tkn = tkn->next;
//         for (int i = 1; i <= num_tokens - 3; i++) {
//           memset(value, '\0', 128);
//           strcpy(value, first_tkn->value);
//           AddObjIndexToArray(obj, first_tkn);
//           obj->num_indices++;
//           strcpy(first_tkn->value, value);
// 
//           tkn = second_tkn;
//           for (int j = 0; j < 2; j++) {
//             memset(value, '\0', 128);
//             strcpy(value, tkn->value);
//             AddObjIndexToArray(obj, tkn);
//             obj->num_indices++;
//             strcpy(tkn->value, value);
//             tkn = tkn->next;
//           }
//           second_tkn = second_tkn->next;
//         }
// 
//         break;
//       }
//       case STATE_UNKNOWN:
//       default: {
//         break;
//       }
//     }
//   }
// 
//   for (int i = 0; i < obj->num_indices / 3; i++) {
//     int base = i * 3;
//     printf("%d/%d/%d ", obj->indices[base].vertex_index, obj->indices[base].tex_coord_index, obj->indices[base].normal_index);
//     printf("%d/%d/%d ", obj->indices[base + 1].vertex_index, obj->indices[base + 1].tex_coord_index, obj->indices[base + 1].normal_index);
//     printf("%d/%d/%d\n", obj->indices[base + 2].vertex_index, obj->indices[base + 2].tex_coord_index, obj->indices[base + 2].normal_index);
//   }
// 
//   CalculateNormals(obj);
// 
//   return obj;
// }

bool loadOBJ(
  const char * path, 
  std::vector<glm::vec3> & out_vertices, 
  std::vector<glm::vec2> & out_uvs,
  std::vector<glm::vec3> & out_normals
){
  printf("Loading OBJ file %s...\n", path);

  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<glm::vec3> temp_vertices; 
  std::vector<glm::vec2> temp_uvs;
  std::vector<glm::vec3> temp_normals;


  FILE * file = fopen(path, "r");
  if( file == NULL ){
    printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
    getchar();
    return false;
  }

  while( 1 ){

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
    }else if ( strcmp( lineHeader, "vt" ) == 0 ){
      glm::vec2 uv;
      fscanf(file, "%f %f\n", &uv.x, &uv.y );
      uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
      temp_uvs.push_back(uv);
    }else if ( strcmp( lineHeader, "vn" ) == 0 ){
      glm::vec3 normal;
      fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
      temp_normals.push_back(normal);
    }else if ( strcmp( lineHeader, "f" ) == 0 ){
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
    }else{
      // Probably a comment, eat up the rest of the line
      char stupidBuffer[1000];
      fgets(stupidBuffer, 1000, file);
    }

  }

  // For each vertex of each triangle
  for( unsigned int i=0; i<vertexIndices.size(); i++ ){

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

#endif
