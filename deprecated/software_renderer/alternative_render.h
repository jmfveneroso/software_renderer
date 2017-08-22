#ifndef ALTERNATIVE_RENDER_H
#define ALTERNATIVE_RENDER_H

#include "vector.h"
#include "vertex.h"
#include "matrix.h"
#include "edge.h"
#include "visual_engine.h"
#include "bitmap.h"
#include "objloader.h"

Bitmap* texture_bmps[10];
float* z_buffer;

void DrawScanLine(Gradients gradients, Edge* left, Edge* right, int j, Bitmap* texture) {
  int x_min = ceil(left->x);
  int x_max = ceil(right->x);
  float x_prestep = x_min - left->x;

  float x_dist = right->x - left->x;
  float tex_xx_step = (right->tex_x - left->tex_x) / x_dist;
  float tex_yx_step = (right->tex_y - left->tex_y) / x_dist;
  float one_over_zx_step = (right->one_over_z - left->one_over_z) / x_dist;
  float depth_x_step = (right->depth - left->depth) / x_dist;
  float light_amt_x_step = (right->light_amt - left->light_amt) / x_dist;

  Vector4f color = VectorAdd(left->color, VectorScalarMul(gradients.color_x_step, x_prestep));

  float tex_x = left->tex_x + tex_xx_step * x_prestep;
  float tex_y = left->tex_y + tex_yx_step * x_prestep;
  float one_over_z = left->one_over_z + one_over_zx_step * x_prestep;
  float depth = left->depth + depth_x_step * x_prestep;
  float light_amt = left->light_amt + light_amt_x_step * x_prestep;

  for (int i = x_min; i < x_max; i++) {
    int index = i + j * 1000;
    if (depth < z_buffer[index]) {
      z_buffer[index] = depth;
      float z = 1.0f / one_over_z;
      int src_x = (int) ((tex_x * z) * (texture->width  - 1) + 0.5f);
      int src_y = (int) ((tex_y * z) * (texture->height - 1) + 0.5f);
   
      unsigned char r = GetComponent(texture, src_x, src_y, 2);
      unsigned char g = GetComponent(texture, src_x, src_y, 1);
      unsigned char b = GetComponent(texture, src_x, src_y, 0);

      DrawPixel(i, j, light_amt * r, light_amt * g, light_amt * b);
    }

    // color = VectorAdd(color, gradients.color_x_step);
    tex_x += tex_xx_step;
    tex_y += tex_yx_step;
    one_over_z += one_over_zx_step;
    depth += depth_x_step;
    light_amt += light_amt_x_step;
  }
}

void ScanEdges(Gradients gradients, Edge* a, Edge* b, bool handedness, Bitmap* texture) {
  Edge* left = a;
  Edge* right = b;
  if (handedness) {
    Edge* tmp = left;
    left = right;
    right = tmp;  
  }
  
  int y_start = b->y_start;
  int y_end   = b->y_end;
  for (int j = y_start; j < y_end; j++) {
    DrawScanLine(gradients, left, right, j, texture);
    EdgeStep(left);
    EdgeStep(right);
  }
}

void ScanTriangle(Vertex min_y_vert, Vertex mid_y_vert, Vertex max_y_vert, bool handedness, Bitmap* texture) {
  Gradients gradients = CreateGradients(min_y_vert, mid_y_vert, max_y_vert);
  Edge top_to_bottom    = CreateEdge(gradients, min_y_vert, max_y_vert, 0);
  Edge top_to_middle    = CreateEdge(gradients, min_y_vert, mid_y_vert, 0);
  Edge middle_to_bottom = CreateEdge(gradients, mid_y_vert, max_y_vert, 1);

  ScanEdges(gradients, &top_to_bottom, &top_to_middle,    handedness, texture);
  ScanEdges(gradients, &top_to_bottom, &middle_to_bottom, handedness, texture);
}

float TriangleAreaTimesTwo(Vertex a, Vertex b, Vertex c) {
  float x1 = b.pos.x - a.pos.x;
  float y1 = b.pos.y - a.pos.y;
  float x2 = c.pos.x - a.pos.x;
  float y2 = c.pos.y - a.pos.y;
  return (x1 * y2) - (x2 * y1);
}

void FillTriangle(Vertex v1, Vertex v2, Vertex v3, Bitmap* texture) {
  Matrix4f screen_space_transform = InitScreenSpaceTransform(500, 300);
  Matrix4f identity = InitIdentity();

  v1 = VectorTransform(screen_space_transform, identity, v1);
  v2 = VectorTransform(screen_space_transform, identity, v2);
  v3 = VectorTransform(screen_space_transform, identity, v3);
  Vertex min_y_vert = VectorPerspectiveDivide(v1);
  Vertex mid_y_vert = VectorPerspectiveDivide(v2);
  Vertex max_y_vert = VectorPerspectiveDivide(v3);

  float area = TriangleAreaTimesTwo(min_y_vert, max_y_vert, mid_y_vert);
  if (area >= 0) return;

  if (max_y_vert.pos.y < mid_y_vert.pos.y) {
    Vertex tmp = max_y_vert;
    max_y_vert = mid_y_vert;
    mid_y_vert = tmp;
  }

  if (mid_y_vert.pos.y < min_y_vert.pos.y) {
    Vertex tmp = mid_y_vert;
    mid_y_vert = min_y_vert;
    min_y_vert = tmp;
  }

  if (max_y_vert.pos.y < mid_y_vert.pos.y) {
    Vertex tmp = max_y_vert;
    max_y_vert = mid_y_vert;
    mid_y_vert = tmp;
  }

  area = TriangleAreaTimesTwo(min_y_vert, max_y_vert, mid_y_vert);
  ScanTriangle(min_y_vert, mid_y_vert, max_y_vert, area >= 0, texture);
}

Vertex* ClipPolygonComponent(Vertex vertices[], int vertices_size, int component_index, float component_factor, int* result_size) {
  Vertex* result = (Vertex*) calloc(0, sizeof(Vertex));
  *result_size = 0;

  Vertex prev_vertex = vertices[vertices_size - 1];
  float prev_component = GetComponent(prev_vertex, component_index) * component_factor;
  bool prev_inside = prev_component <= prev_vertex.pos.w;

  for (int i = 0; i < vertices_size; i++) {
    Vertex current_vertex = vertices[i];
    float current_component = GetComponent(current_vertex, component_index) * component_factor;
    bool current_inside = current_component <= current_vertex.pos.w;

    if (current_inside ^ prev_inside) {
      float lerp_amt = (prev_vertex.pos.w - prev_component) / ((prev_vertex.pos.w - prev_component) - (current_vertex.pos.w - current_component));
      result = (Vertex*) realloc(result, sizeof(Vertex) * ((*result_size) + 1));
      result[(*result_size)++] = LerpVertex(prev_vertex, current_vertex, lerp_amt);
    }
 
    if (current_inside) {
      result = (Vertex*) realloc(result, sizeof(Vertex) * ((*result_size) + 1));
      result[(*result_size)++] = current_vertex;
    } 

    prev_vertex = current_vertex;
    prev_component = current_component;
    prev_inside = current_inside;
  }
 
  return result; 
}

Vertex* ClipPolygonAxis(Vertex* vertices, int* vertices_size, int component_index) {
  int result_size;
  Vertex* auxillary_list = ClipPolygonComponent(vertices, *vertices_size, component_index, 1.0f, vertices_size);
  if (*vertices_size == 0) return NULL;

  return ClipPolygonComponent(auxillary_list, *vertices_size, component_index, -1.0f, vertices_size);
}

void DrawTriangle(Vertex v1, Vertex v2, Vertex v3, Bitmap* texture) {
  if (IsInsideViewFrustum(v1) && IsInsideViewFrustum(v2) && IsInsideViewFrustum(v3)) {
    FillTriangle(v1, v2, v3, texture);
    return;
  }

  Vertex* vertices = (Vertex*) malloc(sizeof(Vertex) * 3);
  vertices[0] = v1;
  vertices[1] = v2;
  vertices[2] = v3;
  int vertices_size = 3;

  vertices = ClipPolygonAxis(vertices, &vertices_size, 0);
  if (vertices_size <= 0) return;
  vertices = ClipPolygonAxis(vertices, &vertices_size, 1);
  if (vertices_size <= 0) return;
  vertices = ClipPolygonAxis(vertices, &vertices_size, 2);
  if (vertices_size <= 0) return;

  Vertex initial_vertex = vertices[0];
  for (int i = 1; i < vertices_size - 1; i++) {
    FillTriangle(initial_vertex, vertices[i], vertices[i + 1], texture);
  }
}

void DrawMyTriangle() {
  Matrix4f projection = InitPerspective(1.22173, 1000.0f / 600.0f, 0.1f, 1000.0f);
  static float rot_counter = 0.0f;
  rot_counter += 0.03;

  Matrix4f translation = InitTranslation(0.0f, 0.0f, 5.0f);
  Matrix4f rotation    = InitAxisRotation(rot_counter, rot_counter, rot_counter);
  // Matrix4f transform   = MatrixMul(projection, MatrixMul(translation, rotation));
  Matrix4f transform   = MatrixMul(translation, rotation);
  Matrix4f mvp = MatrixMul(projection, transform);

  Vertex min_y_vert = VectorTransform(mvp, transform, CreateVertex(CreateVector4f(1, -1, 0, 1), CreateVector4f(0, 0, 1, 0), CreateVector4f(1.0f, 0, 0, 0)));
  Vertex mid_y_vert = VectorTransform(mvp, transform, CreateVertex(CreateVector4f(0, 1, 0, 1), CreateVector4f(0, 1, 0, 0), CreateVector4f(0.5f, 1.0f, 0, 0)));
  Vertex max_y_vert = VectorTransform(mvp, transform, CreateVertex(CreateVector4f(-1, -1, 0, 1), CreateVector4f(1, 0, 0, 0), CreateVector4f(0, 0, 0, 0)));

  FillTriangle(max_y_vert, mid_y_vert, min_y_vert, texture_bmps[0]);
}

ObjModel* mesh;
void DrawMesh(ObjModel* model, Matrix4f view_projection, Matrix4f transform, Bitmap* texture) {
  Matrix4f mvp = MatrixMul(view_projection, transform);

  for (int i = 0; i < model->num_indices; i += 3) {
    Vector4f v1_ = model->positions[model->indices[i    ].vertex_index];
    Vector4f v2_ = model->positions[model->indices[i + 1].vertex_index];
    Vector4f v3_ = model->positions[model->indices[i + 2].vertex_index];
    Vector4f tex1 = model->tex_coords[model->indices[i    ].tex_coord_index];
    Vector4f tex2 = model->tex_coords[model->indices[i + 1].tex_coord_index];
    Vector4f tex3 = model->tex_coords[model->indices[i + 2].tex_coord_index];
    Vector4f normal1 = model->normals[model->indices[i    ].normal_index];
    Vector4f normal2 = model->normals[model->indices[i + 1].normal_index];
    Vector4f normal3 = model->normals[model->indices[i + 2].normal_index];

    Vertex v1 = VectorTransform(mvp, transform, CreateVertex(v1_, normal1, tex1));
    Vertex v2 = VectorTransform(mvp, transform, CreateVertex(v2_, normal2, tex2));
    Vertex v3 = VectorTransform(mvp, transform, CreateVertex(v3_, normal3, tex3));

    // FillTriangle(v1, v2, v3, texture);
    DrawTriangle(v1, v2, v3, texture);
  }
}

void ClearZBuffer() {
  for (int i = 0; i < 1000 * 600; i++) {
    z_buffer[i] = 999999.0f;
  }
}

void DrawFloorDoMal() {
  Matrix4f projection = InitPerspective(1.22173, 1000.0f / 600.0f, 0.1f, 1000.0f);
  Matrix4f translation = InitTranslation(0.0f, 0.0f, 5.0f);
  Matrix4f transform   = translation;
  Matrix4f mvp         = MatrixMul(projection, translation);
  int half_size = 20;
  for (int i = -half_size; i <= half_size; i++) {
    for (int j = -half_size; j <= half_size; j++) {
      
      float y1 = 3 * sin((float) (i + 1) * 0.5f) + 3 * sin((float) (j + 0) * 0.5f) - 5.0f;
      Vertex v1 = VectorTransform(mvp, transform, CreateVertex(CreateVector4f(i + 1, y1, j + 0, 1), CreateVector4f(1, 0, 0, 0), CreateVector4f(1, 0, 0, 0)));
      float y2 = 3 * sin((float) (i + 0) * 0.5f) + 3 * sin((float) (j + 0) * 0.5f) - 5.0f;
      Vertex v2 = VectorTransform(mvp, transform, CreateVertex(CreateVector4f(i + 0, y2, j + 0, 1), CreateVector4f(0, 1, 0, 0), CreateVector4f(0, 0, 0, 0)));
      float y3 = 3 * sin((float) (i + 0) * 0.5f) + 3 * sin((float) (j + 1) * 0.5f) - 5.0f;
      Vertex v3 = VectorTransform(mvp, transform, CreateVertex(CreateVector4f(i + 0, y3, j + 1, 1), CreateVector4f(0, 0, 1, 0), CreateVector4f(0, 1, 0, 0)));
      float y4 = 3 * sin((float) (i + 1) * 0.5f) + 3 * sin((float) (j + 1) * 0.5f) - 5.0f;
      Vertex v4 = VectorTransform(mvp, transform, CreateVertex(CreateVector4f(i + 1, y4, j + 1, 1), CreateVector4f(0, 0, 1, 0), CreateVector4f(1.0f, 1.0f, 0, 0)));
      DrawTriangle(v1, v2, v3, texture_bmps[0]);
      DrawTriangle(v1, v3, v4, texture_bmps[0]);
    }
  }
}

void InitRender() {
  z_buffer = (float*) malloc(1000 * 600 * sizeof(float));
  // texture_bmps[0] = CreateBitmap("textures/dirt.bmp");
  texture_bmps[0] = CreateBitmap("textures/bricks.bmp");
  // mesh = CreateObjModel("res/monkey0.obj");
  mesh = CreateObjModel("res/pole.obj");
  // mesh = CreateObjModel("res/square.obj");
  // mesh = CreateObjModel("res/icosphere.obj");

}

void Render() {
  ClearProjectionScreen();
  ClearZBuffer();
  // DrawTriangle();

  static float rot_counter = 0.0f;
  rot_counter += 0.03f;

  Matrix4f projection  = InitPerspective(1.22173, 1000.0f / 600.0f, 0.1f, 1000.0f);
  Matrix4f translation = InitTranslation(0.0f, 0.0f, 10.0f);
  Matrix4f rotation    = InitAxisRotation(0, rot_counter, 0);
  // Matrix4f transform   = MatrixMul(projection, MatrixMul(translation, rotation));
  Matrix4f transform   = MatrixMul(translation, rotation);
  DrawMesh(mesh, projection, transform, texture_bmps[0]);
  // DrawFloorDoMal();
  // DrawMyTriangle();
}

#endif
