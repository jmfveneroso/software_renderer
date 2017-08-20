#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <stdio.h>
#include "vector.h"

typedef struct {
  int vertex_index;
  int tex_coord_index;
  int normal_index;
} ObjIndex;

typedef struct {
  Vector4f* positions;
  Vector4f* tex_coords;
  Vector4f* normals;
  ObjIndex* indices;
  size_t num_positions;
  size_t num_tex_coords;
  size_t num_normals;
  size_t num_indices;
  bool has_tex_coords;
  bool has_normals;
} ObjModel;

typedef struct Token {
  char value[128];
  size_t size;
  Token* next;
} Token;

Vector4f* AddVectorToArray(Vector4f* arr, size_t size, Vector4f v) {
  arr = (Vector4f*) realloc(arr, sizeof(Vector4f) * (size + 1));
  arr[size] = v;
  return arr;
}

void AddObjIndexToArray(ObjModel* model, Token* tk) { 
  bool has_value[3];
  int values[3];

  char* tkn_begin = tk->value;
  char* tkn_end   = NULL;
  for (int i = 0; i < 3; i++) {
    tkn_end = tkn_begin;
    int reached_end = 0;
    while (*tkn_end != '/' && *tkn_end != '\0') tkn_end++;
    if (*tkn_end == '\0') reached_end = 1;
    *tkn_end = '\0';
     
    has_value[i] = tkn_begin[0] != '\0';
    if (has_value[i]) values[i] = atoi(tkn_begin);
    else values[i] = 0.0f;
    tkn_begin = tkn_end + (1 - reached_end);
  }

  ObjIndex obj_idx;
  obj_idx.vertex_index = values[0] - 1;

  if (has_value[1]) { 
    model->has_tex_coords = true;
    obj_idx.tex_coord_index = values[1] - 1;
  }

  if (has_value[2]) { 
    model->has_normals = true;
    obj_idx.normal_index = values[2] - 1;
  }

  model->indices = (ObjIndex*) realloc(model->indices, sizeof(ObjIndex) * (model->num_indices + 1));
  model->indices[model->num_indices] = obj_idx;
}

bool ObjIndexEquals(ObjIndex* a, ObjIndex* b) {
  return a->vertex_index    == b->vertex_index    &&
         a->tex_coord_index == b->tex_coord_index &&
         a->normal_index    == b->normal_index;
}

bool ObjIndexHash(ObjIndex* obj) {
  int base = 17;
  int multiplier = 31;

  int result = base;
  result = multiplier * result + obj->vertex_index;
  result = multiplier * result + obj->tex_coord_index;
  result = multiplier * result + obj->normal_index;
  return result;
}

#define STATE_UNKNOWN 0
#define STATE_V       1
#define STATE_VT      2
#define STATE_VN      3
#define STATE_F       4

size_t ReadLine(FILE* file, char* buffer) {
  size_t num_read = 0;
  char c;
  while (fread(&c, sizeof(char), 1, file) == 1) {
    if (c == '\n') break;
    buffer[num_read] = c;
    num_read++;
  }
  buffer[num_read] = '\0';
  return num_read;
}

Token* SplitTokens(char* buffer, size_t* num_tokens) {
  Token* first_token = NULL;
  Token* cur_token = NULL;
  bool at_space = true;
  size_t pos = 0;
  (*num_tokens) = 0;
  char* c;
  for (c = buffer; *c != '\0'; c++) {
    if (*c == ' ' || *c == '\t') {
      if (at_space) continue;
      Token* new_token = (Token*) malloc(sizeof(Token));
      new_token->next = NULL;
      cur_token->next = new_token;
      cur_token->size = 0;
      cur_token = new_token;
      pos = 0;
      at_space = true;
      (*num_tokens)++;
      continue;
    } else if (first_token == NULL) {
      first_token = (Token*) malloc(sizeof(Token));
      first_token->next = NULL;
      first_token->size = 0;
      cur_token = first_token;
      (*num_tokens)++;
    }

    cur_token->value[pos++] = *c; 
    cur_token->size++; 
    at_space = false;
  }
  return first_token;
}

ObjModel* CreateObjModel(const char filename[]) {
  ObjModel* obj = (ObjModel*) malloc(sizeof(ObjModel));
  obj->positions = (Vector4f*) calloc(0, sizeof(Vector4f));
  obj->tex_coords = (Vector4f*) calloc(0, sizeof(Vector4f));
  obj->normals = (Vector4f*) calloc(0, sizeof(Vector4f));
  obj->indices = (ObjIndex*) calloc(0, sizeof(ObjIndex));
  obj->num_positions = 0;
  obj->num_tex_coords = 0;
  obj->num_normals = 0;
  obj->num_indices = 0;
  obj->indices = NULL;
  obj->has_tex_coords = false;
  obj->has_normals = false;

  FILE* file = fopen(filename, "rb");
  if (!file) {
    printf("Problem opening file %s.\n", filename);
    return NULL;
  }

  char buffer[128];
  while (ReadLine(file, buffer) > 0) {
    size_t num_tokens = 0;
    Token* tkn = SplitTokens(buffer, &num_tokens);
    if (num_tokens == 0) continue;
    if (tkn->value[0] == '#') continue;

    printf("Line: %s num tks: %lu\n", buffer, num_tokens);
    short state = STATE_UNKNOWN;
    if      (strcmp(tkn->value, "v" ) == 0) state = STATE_V;
    else if (strcmp(tkn->value, "vt") == 0) state = STATE_VT;
    else if (strcmp(tkn->value, "vn") == 0) state = STATE_VN;
    else if (strcmp(tkn->value, "f" ) == 0) state = STATE_F;

    tkn = tkn->next;
    switch (state) {
      case STATE_V: {
        float x = atof(tkn->value); tkn = tkn->next;
        float y = atof(tkn->value); tkn = tkn->next;
        float z = atof(tkn->value); tkn = tkn->next;
        obj->positions = AddVectorToArray(obj->positions, obj->num_positions, CreateVector4f(x, y, z, 1));
        obj->num_positions++;
        break;
      }
      case STATE_VT: {
        float x = atof(tkn->value); tkn = tkn->next;
        float y = 1.0f - atof(tkn->value); tkn = tkn->next;
        obj->tex_coords = AddVectorToArray(obj->tex_coords, obj->num_tex_coords, CreateVector4f(x, y, 0, 0));
        obj->num_tex_coords++;
        break;
      }
      case STATE_VN: {
        float x = atof(tkn->value); tkn = tkn->next;
        float y = atof(tkn->value); tkn = tkn->next;
        float z = atof(tkn->value); tkn = tkn->next;
        obj->normals = AddVectorToArray(obj->normals, obj->num_normals, CreateVector4f(x, y, z, 0));
        obj->num_normals++;
        break;
      }
      case STATE_F: {
        char value[128]; 
        Token* first_tkn = tkn;
        Token* second_tkn = tkn->next;
        for (int i = 1; i <= num_tokens - 3; i++) {
          memset(value, '\0', 128);
          strcpy(value, first_tkn->value);
          AddObjIndexToArray(obj, first_tkn);
          obj->num_indices++;
          strcpy(first_tkn->value, value);

          tkn = second_tkn;
          for (int j = 0; j < 2; j++) {
            memset(value, '\0', 128);
            strcpy(value, tkn->value);
            AddObjIndexToArray(obj, tkn);
            obj->num_indices++;
            strcpy(tkn->value, value);
            tkn = tkn->next;
          }
          second_tkn = second_tkn->next;
        }

        break;
      }
      case STATE_UNKNOWN:
      default: {
        break;
      }
    }
  }

  for (int i = 0; i < obj->num_indices / 3; i++) {
    int base = i * 3;
    printf("%d/%d/%d ", obj->indices[base].vertex_index, obj->indices[base].tex_coord_index, obj->indices[base].normal_index);
    printf("%d/%d/%d ", obj->indices[base + 1].vertex_index, obj->indices[base + 1].tex_coord_index, obj->indices[base + 1].normal_index);
    printf("%d/%d/%d\n", obj->indices[base + 2].vertex_index, obj->indices[base + 2].tex_coord_index, obj->indices[base + 2].normal_index);
  }

  return obj;
}

// typedef struct {
//   Vector4f* positions;
//   Vector4f* tex_coords;
//   Vector4f* normals;
//   Vector4f* tangents;
//   int* indices;
//   size_t num_positions;
//   size_t num_tex_coords;
//   size_t num_normals;
//   size_t num_tangents;
//   size_t num_indices;
// } IndexedModel;

// void CalcNormals(IndexedModel* model) {
//   // for (int i = 0; i < m_indices.size(); i += 3) {
//   //   int i0 = m_indices.get(i);
//   //   int i1 = m_indices.get(i + 1);
//   //   int i2 = m_indices.get(i + 2);
// 
//   //   Vector4f v1 = m_positions.get(i1).Sub(m_positions.get(i0));
//   //   Vector4f v2 = m_positions.get(i2).Sub(m_positions.get(i0));
// 
//   //   Vector4f normal = v1.Cross(v2).Normalized();
// 
//   //   m_normals.set(i0, m_normals.get(i0).Add(normal));
//   //   m_normals.set(i1, m_normals.get(i1).Add(normal));
//   //   m_normals.set(i2, m_normals.get(i2).Add(normal));
//   // }
// 
//   // for (int i = 0; i < m_normals.size(); i++)
//   //   m_normals.set(i, m_normals.get(i).Normalized());
// }
// 
// void CalcTangents(IndexedModel* model) {
//   // for (int i = 0; i < m_indices.size(); i += 3) {
//   //   int i0 = m_indices.get(i);
//   //   int i1 = m_indices.get(i + 1);
//   //   int i2 = m_indices.get(i + 2);
// 
//   //   Vector4f edge1 = m_positions.get(i1).Sub(m_positions.get(i0));
//   //   Vector4f edge2 = m_positions.get(i2).Sub(m_positions.get(i0));
// 
//   //   float deltaU1 = m_texCoords.get(i1).GetX() - m_texCoords.get(i0).GetX();
//   //   float deltaV1 = m_texCoords.get(i1).GetY() - m_texCoords.get(i0).GetY();
//   //   float deltaU2 = m_texCoords.get(i2).GetX() - m_texCoords.get(i0).GetX();
//   //   float deltaV2 = m_texCoords.get(i2).GetY() - m_texCoords.get(i0).GetY();
// 
//   //   float dividend = (deltaU1*deltaV2 - deltaU2*deltaV1);
//   //   float f = dividend == 0 ? 0.0f : 1.0f/dividend;
// 
//   //   Vector4f tangent = new Vector4f(
//   //       f * (deltaV2 * edge1.GetX() - deltaV1 * edge2.GetX()),
//   //       f * (deltaV2 * edge1.GetY() - deltaV1 * edge2.GetY()),
//   //       f * (deltaV2 * edge1.GetZ() - deltaV1 * edge2.GetZ()),
//   //       0);
//   //   
//   //   m_tangents.set(i0, m_tangents.get(i0).Add(tangent));
//   //   m_tangents.set(i1, m_tangents.get(i1).Add(tangent));
//   //   m_tangents.set(i2, m_tangents.get(i2).Add(tangent));
//   // }
// 
//   // for(int i = 0; i < m_tangents.size(); i++)
//   //   m_tangents.set(i, m_tangents.get(i).Normalized());
// }

// IndexedModel* CreateIndexedModel() {
//   IndexedModel* result = (IndexedModel*) malloc(sizeof(IndexedModel));
//   result->positions = (Vector4f*) calloc(0, sizeof(Vector4f));
//   result->tex_coords = (Vector4f*) calloc(0, sizeof(Vector4f));
//   result->normals = (Vector4f*) calloc(0, sizeof(Vector4f));
//   result->tangents = (Vector4f*) calloc(0, sizeof(Vector4f));
//   result->indices = (int*) calloc(0, sizeof(int));
//   result->num_positions = 0;
//   result->num_tex_coords = 0;
//   result->num_normals = 0;
//   result->num_tangents= 0;
//   result->num_indices = 0;
//   return result;
// }
// 
// IndexedModel* ObjToIndexedModel(ObjModel* model) {
//   IndexedModel* result = CreateIndexedModel();
//   IndexedModel* normal_model = CreateIndexedModel();
// 
//   int result_index_map = (*int) malloc(sizeof(int) * model->indices);
//   int normal_index_map = (*int) malloc(sizeof(int) * model->indices);
//   int index_map = (*int) malloc(sizeof(int) * model->indices);
// 
//   int vertex_index;
//   int tex_coord_index;
//   int normal_index;
//   for (int i = 0; i < model->num_indices; i++) {
//     ObjIndex idx = model->num_indices[i];
//     Vector4f current_pos = model->positions[idx.vertex_index];
//     Vector4f current_tex_coord;
//     Vector4f current_normal;
// 
//     if (model->has_tex_coords)
//       current_tex_coord = model->tex_coords[idx.tex_coord_index];
//     else
//       current_tex_coord = CreateVector4f(0, 0, 0, 0);
// 
//     if (model->has_normals)
//       current_normal = model->normals[idx.normal_index];
//     else
//       current_normal = CreateVector4f(0, 0, 0, 0);
// 
//     int model_vertex_index = result_index_map[i];
// 
//     if (model_vertex_index == -1) {
//       model_vertex_index = result->positions;
//       result_index_map[i] = model_vertex_index;
// 
//       result->positions.add(current_pos);
//       result->tex_coords.add(currentTexCoord);
//       if (model->has_normals)
//         result->normals().add(currentNormal);
//     }
// 
//     int normal_model_index = normal_index_map[idx.vertex_index];
//     if (normal_model_index == -1) {
//       normal_model_index = normal_model->positions().size();
//       normal_index_map[idx.vertex_index] = normal_model_index;
// 
//       normal_model->positions().add(currentPosition);
//       normal_model->tex_coords().add(currentTexCoord);
//       normal_model->normals().add(currentNormal);
//       normal_model->tangents().add(new Vector4f(0,0,0,0));
//     }
// 
//     result->indices.add(modelVertexIndex);
//     normal_model->indices.add(normalModelIndex);
//     index_map[model_vertex_index] = normal_model_index;
//   }
// 
//   if (!model->has_normals) {
//     CalcNormals(normal_model);
// 
//     for (int i = 0; i < result.GetPositions().size(); i++)
//       result.GetNormals().add(normalModel.GetNormals().get(indexMap.get(i)));
//   }
// 
//   CalcTangents(normal_model);
// 
//   for (int i = 0; i < result.GetPositions().size(); i++)
//     result.GetTangents().add(normalModel.GetTangents().get(indexMap.get(i)));
// 
//   return result;
//   
// }

#endif
