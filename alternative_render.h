#ifndef ALTERNATIVE_RENDER_H
#define ALTERNATIVE_RENDER_H

#include "player.h"
#include "level.h"
#include "bsp_tree.h"
#include "sector.h"
#include "visual_engine.h"

//====================
// Vector
//====================

struct Vector4f {
  float x, y, z, w;
};

Vector4f CreateVector4f(float x, float y, float z, float w) {
  Vector4f v; 
  v.x = x; v.y = y;
  v.z = z; v.w = w;
  return v;
}

//====================
// Vertex
//====================

struct Vertex {
  Vector4f pos;
  Vector4f color;
};

Vertex CreateVertex(Vector4f pos, Vector4f color) {
  Vertex v;
  v.pos = pos;
  v.color = color;
  return v;
}

float VectorLength(Vector4f v) {
  return (float) sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

float VectorMax(Vector4f v) {
  return (float) fmax(fmax(v.x, v.y), fmax(v.z, v.w));
}

float VectorDot(Vector4f l, Vector4f r) {
  return (float) l.x * r.x + l.y * r.y + l.z * r.z + l.w * r.w;
}

Vector4f VectorCross(Vector4f l, Vector4f r) {
  float x_ = l.y * r.z - l.z * r.y;
  float y_ = l.z * r.x - l.x * r.z;
  float z_ = l.x * r.y - l.y * r.x;

  return CreateVector4f(x_, y_, z_, 0);
}

Vector4f VectorNormalized(Vector4f v) {
  float length = VectorLength(v);
  return CreateVector4f(v.x / length, v.y / length, v.z / length, v.w / length);
}

Vector4f VectorAdd(Vector4f l, Vector4f r) {
  return CreateVector4f(l.x + r.x, l.y + r.y, l.z + r.z, l.w + r.w);
}

Vector4f VectorScalarAdd(Vector4f v, float s) {
  return CreateVector4f(v.x + s, v.y + s, v.z + s, v.w + s);
}

Vector4f VectorSub(Vector4f l, Vector4f r) {
  return CreateVector4f(l.x - r.x, l.y - r.y, l.z - r.z, l.w - r.w);
}

Vector4f VectorScalarSub(Vector4f v, float s) {
  return CreateVector4f(v.x - s, v.y - s, v.z - s, v.w - s);
}

Vector4f VectorMul(Vector4f l, Vector4f r) {
  return CreateVector4f(l.x * r.x, l.y * r.y, l.z * r.z, l.w * r.w);
}

Vector4f VectorScalarMul(Vector4f v, float s) {
  return CreateVector4f(v.x * s, v.y * s, v.z * s, v.w * s);
}

Vector4f VectorDiv(Vector4f l, Vector4f r) {
  return CreateVector4f(l.x / r.x, l.y / r.y, l.z / r.z, l.w / r.w);
}

Vector4f VectorScalarDiv(Vector4f v, float s) {
  return CreateVector4f(v.x / s, v.y / s, v.z / s, v.w / s);
}

Vector4f VectorAbs(Vector4f v) {
  return CreateVector4f(fabs(v.x), fabs(v.y), fabs(v.z), fabs(v.w));
}

bool VectorEquals(Vector4f l, Vector4f r) {
  bool x_ = fabs(l.x - r.x) < 0.00001f;
  bool y_ = fabs(l.y - r.y) < 0.00001f;
  bool z_ = fabs(l.z - r.z) < 0.00001f;
  bool w_ = fabs(l.w - r.w) < 0.00001f;
  return x_ && y_ && w_ && z_;
}

//====================
// Matrix
//====================

struct Matrix4f {
  float m[4][4]; 
};

Matrix4f InitIdentity() {
  Matrix4f mat;
  mat.m[0][0] = 1; mat.m[0][1] = 0; mat.m[0][2] = 0; mat.m[0][3] = 0;
  mat.m[1][0] = 0; mat.m[1][1] = 1; mat.m[1][2] = 0; mat.m[1][3] = 0;
  mat.m[2][0] = 0; mat.m[2][1] = 0; mat.m[2][2] = 1; mat.m[2][3] = 0;
  mat.m[3][0] = 0; mat.m[3][1] = 0; mat.m[3][2] = 0; mat.m[3][3] = 1;
  return mat;
}

Matrix4f InitScreenSpaceTransform(float half_width, float half_height) {
  Matrix4f mat;
  mat.m[0][0] = half_width; mat.m[0][1] = 0;             mat.m[0][2] = 0; mat.m[0][3] = half_width;
  mat.m[1][0] = 0;          mat.m[1][1] = -half_height;  mat.m[1][2] = 0; mat.m[1][3] = half_height;
  mat.m[2][0] = 0;          mat.m[2][1] = 0;             mat.m[2][2] = 1; mat.m[2][3] = 0;
  mat.m[3][0] = 0;          mat.m[3][1] = 0;             mat.m[3][2] = 0; mat.m[3][3] = 1;
  return mat;
}

Matrix4f InitTranslation(float x, float y, float z) {
  Matrix4f mat;
  mat.m[0][0] = 1; mat.m[0][1] = 0; mat.m[0][2] = 0; mat.m[0][3] = x;
  mat.m[1][0] = 0; mat.m[1][1] = 1; mat.m[1][2] = 0; mat.m[1][3] = y;
  mat.m[2][0] = 0; mat.m[2][1] = 0; mat.m[2][2] = 1; mat.m[2][3] = z;
  mat.m[3][0] = 0; mat.m[3][1] = 0; mat.m[3][2] = 0; mat.m[3][3] = 1;
  return mat;
}

Matrix4f InitRotation(float x, float y, float z, float angle) {
  float sin_ = sin(angle);
  float cos_ = cos(angle);

  Matrix4f mat;
  mat.m[0][0] = cos_+x*x*(1-cos_);   mat.m[0][1] = x*y*(1-cos_)-z*sin_; mat.m[0][2] = x*z*(1-cos_)+y*sin_; mat.m[0][3] = 0;
  mat.m[1][0] = y*x*(1-cos_)+z*sin_; mat.m[1][1] = cos_+y*y*(1-cos_);   mat.m[1][2] = y*z*(1-cos_)-x*sin_; mat.m[1][3] = 0;
  mat.m[2][0] = z*x*(1-cos_)-y*sin_; mat.m[2][1] = z*y*(1-cos_)+x*sin_; mat.m[2][2] = cos_+z*z*(1-cos_);   mat.m[2][3] = 0;
  mat.m[3][0] = 0;                   mat.m[3][1] = 0;                   mat.m[3][2] = 0;                   mat.m[3][3] = 1;
  return mat;
}

Matrix4f MatrixMul(Matrix4f l, Matrix4f r) {
  Matrix4f mat;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      mat.m[i][j] = l.m[i][0] * r.m[0][j] + l.m[i][1] * r.m[1][j] + 
                    l.m[i][2] * r.m[2][j] + l.m[i][3] * r.m[3][j];
    }
  }
  return mat;
}

Matrix4f InitAxisRotation(float x, float y, float z) {
  Matrix4f rx;
  Matrix4f ry;
  Matrix4f rz;

  rz.m[0][0] = (float) cos(z); rz.m[0][1] = -(float) sin(z); rz.m[0][2] = 0;               rz.m[0][3] = 0;
  rz.m[1][0] = (float) sin(z); rz.m[1][1] = (float) cos(z);  rz.m[1][2] = 0;               rz.m[1][3] = 0;
  rz.m[2][0] = 0;              rz.m[2][1] = 0;               rz.m[2][2] = 1;               rz.m[2][3] = 0;
  rz.m[3][0] = 0;              rz.m[3][1] = 0;               rz.m[3][2] = 0;               rz.m[3][3] = 1;

  rx.m[0][0] = 1;              rx.m[0][1] = 0;               rx.m[0][2] = 0;               rx.m[0][3] = 0;
  rx.m[1][0] = 0;              rx.m[1][1] = (float) cos(x);  rx.m[1][2] = -(float) sin(x); rx.m[1][3] = 0;
  rx.m[2][0] = 0;              rx.m[2][1] = (float) sin(x);  rx.m[2][2] = (float) cos(x);  rx.m[2][3] = 0;
  rx.m[3][0] = 0;              rx.m[3][1] = 0;               rx.m[3][2] = 0;               rx.m[3][3] = 1;

  ry.m[0][0] = (float) cos(y); ry.m[0][1] = 0;               ry.m[0][2] = -(float) sin(y); ry.m[0][3] = 0;
  ry.m[1][0] = 0;              ry.m[1][1] = 1;               ry.m[1][2] = 0;               ry.m[1][3] = 0;
  ry.m[2][0] = (float) sin(y); ry.m[2][1] = 0;               ry.m[2][2] = (float) cos(y);  ry.m[2][3] = 0;
  ry.m[3][0] = 0;              ry.m[3][1] = 0;               ry.m[3][2] = 0;               ry.m[3][3] = 1;

  return MatrixMul(rz, MatrixMul(ry, rx));
}

Matrix4f InitScale(float x, float y, float z) {
  Matrix4f mat;
  mat.m[0][0] = x; mat.m[0][1] = 0; mat.m[0][2] = 0; mat.m[0][3] = 0;
  mat.m[1][0] = 0; mat.m[1][1] = y; mat.m[1][2] = 0; mat.m[1][3] = 0;
  mat.m[2][0] = 0; mat.m[2][1] = 0; mat.m[2][2] = z; mat.m[2][3] = 0;
  mat.m[3][0] = 0; mat.m[3][1] = 0; mat.m[3][2] = 0; mat.m[3][3] = 1;
  return mat;
}

Matrix4f InitPerspective(float fov, float aspect_ratio, float z_near, float z_far) {
  Matrix4f mat;
  float tan_half_FOV = (float) tan(fov / 2.0f);
  float z_range = z_near - z_far;

  mat.m[0][0] = 1.0f / (tan_half_FOV * aspect_ratio); mat.m[0][1] = 0;                   mat.m[0][2] = 0;                        mat.m[0][3] = 0;
  mat.m[1][0] = 0;                                    mat.m[1][1] = 1.0f / tan_half_FOV; mat.m[1][2] = 0;                        mat.m[1][3] = 0;
  mat.m[2][0] = 0;                                    mat.m[2][1] = 0;                   mat.m[2][2] = (-z_near -z_far)/z_range; mat.m[2][3] = 2 * z_far * z_near / z_range;
  mat.m[3][0] = 0;                                    mat.m[3][1] = 0;                   mat.m[3][2] = 1;                        mat.m[3][3] = 0;
  return mat;
}

Matrix4f InitVectorRotation(Vector4f forward, Vector4f up, Vector4f right) {
  Matrix4f mat;
  Vector4f f = forward;
  Vector4f r = right;
  Vector4f u = up;

  mat.m[0][0] = r.x; mat.m[0][1] = r.y; mat.m[0][2] = r.z; mat.m[0][3] = 0;
  mat.m[1][0] = u.x; mat.m[1][1] = u.y; mat.m[1][2] = u.z; mat.m[1][3] = 0;
  mat.m[2][0] = f.x; mat.m[2][1] = f.y; mat.m[2][2] = f.z; mat.m[2][3] = 0;
  mat.m[3][0] = 0;   mat.m[3][1] = 0;   mat.m[3][2] = 0;   mat.m[3][3] = 1;
  return mat;
}

Matrix4f InitCrossRotation(Vector4f forward, Vector4f up) {
  Vector4f f = VectorNormalized(forward);
  Vector4f r = VectorNormalized(up);
  r = VectorCross(r, f);

  Vector4f u = VectorCross(f, r);
  return InitVectorRotation(f, u, r);
}

Vertex VectorTransform(Matrix4f mat, Vertex v) {
  Vector4f r = v.pos;
  float x = mat.m[0][0] * r.x + mat.m[0][1] * r.y + mat.m[0][2] * r.z + mat.m[0][3] * r.w;
  float y = mat.m[1][0] * r.x + mat.m[1][1] * r.y + mat.m[1][2] * r.z + mat.m[1][3] * r.w;
  float z = mat.m[2][0] * r.x + mat.m[2][1] * r.y + mat.m[2][2] * r.z + mat.m[2][3] * r.w;
  float w = mat.m[3][0] * r.x + mat.m[3][1] * r.y + mat.m[3][2] * r.z + mat.m[3][3] * r.w;
  return CreateVertex(CreateVector4f(x, y, z, w), v.color);
}

Vertex VectorPerspectiveDivide(Vertex r) {
  Vector4f v = r.pos;
  return CreateVertex(CreateVector4f(v.x / v.w, v.y / v.w, v.z / v.w, v.w), r.color);
}

//====================
// Render
//====================

struct Edge {
  float x;
  float x_step;
  int y_start;  
  int y_end;
};

Edge CreateEdge(Vector4f min_y_vert, Vector4f max_y_vert) {
  Edge e;
  e.y_start = (int) ceil(min_y_vert.y);
  e.y_end   = (int) ceil(max_y_vert.y);

  float y_dist = max_y_vert.y - min_y_vert.y; 
  float x_dist = max_y_vert.x - min_y_vert.x; 

  float y_prestep = e.y_start - min_y_vert.y;
  e.x_step = (float) x_dist / (float) y_dist;
  e.x = (float) min_y_vert.x + y_prestep * e.x_step;
  return e;
}

void EdgeStep(Edge* e) {
  e->x += e->x_step;
}

void DrawScanLine(Edge* left, Edge* right, int j) {
  int x_min = ceil(left->x);
  int x_max = ceil(right->x);
  for (int i = x_min; i < x_max; i++) {
    DrawPixel(i, j, 0xFF, 0xFF, 0xFF);
  }
}

void ScanEdges(Edge* a, Edge* b, bool handedness) {
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
    DrawScanLine(left, right, j);
    EdgeStep(left);
    EdgeStep(right);
  }
}

void ScanTriangle(Vector4f min_y_vert, Vector4f mid_y_vert, Vector4f max_y_vert, bool handedness) {
  Edge top_to_bottom    = CreateEdge(min_y_vert, max_y_vert);
  Edge top_to_middle    = CreateEdge(min_y_vert, mid_y_vert);
  Edge middle_to_bottom = CreateEdge(mid_y_vert, max_y_vert);

  ScanEdges(&top_to_bottom, &top_to_middle,    handedness);
  ScanEdges(&top_to_bottom, &middle_to_bottom, handedness);
}

float TriangleAreaTimesTwo(Vector4f a, Vector4f b, Vector4f c) {
  float x1 = b.x - a.x;
  float y1 = b.y - a.y;

  float x2 = c.x - a.x;
  float y2 = c.y - a.y;

  return (x1 * y2) - (x2 * y1);
}

void FillTriangle(Vertex v1, Vertex v2, Vertex v3) {
  Matrix4f screen_space_transform = InitScreenSpaceTransform(500, 300);
  v1 = VectorTransform(screen_space_transform, v1);
  v2 = VectorTransform(screen_space_transform, v2);
  v3 = VectorTransform(screen_space_transform, v3);
  Vector4f min_y_vert = VectorPerspectiveDivide(v1).pos;
  Vector4f mid_y_vert = VectorPerspectiveDivide(v2).pos;
  Vector4f max_y_vert = VectorPerspectiveDivide(v3).pos;

  if (max_y_vert.y < mid_y_vert.y) {
    Vector4f tmp = max_y_vert;
    max_y_vert = mid_y_vert;
    mid_y_vert = tmp;
  }

  if (mid_y_vert.y < min_y_vert.y) {
    Vector4f tmp = mid_y_vert;
    mid_y_vert = min_y_vert;
    min_y_vert = tmp;
  }

  if (max_y_vert.y < mid_y_vert.y) {
    Vector4f tmp = max_y_vert;
    max_y_vert = mid_y_vert;
    mid_y_vert = tmp;
  }

  float area = TriangleAreaTimesTwo(min_y_vert, max_y_vert, mid_y_vert);
  ScanTriangle(min_y_vert, mid_y_vert, max_y_vert, area >= 0);
}

void DrawTriangle() {
  Matrix4f projection = InitPerspective(1.22173, 1000.0f / 600.0f, 0.1f, 1000.0f);
  static float rot_counter = 0.0f;
  rot_counter += 0.01;

  Matrix4f translation = InitTranslation(0.0f, 0.0f, 5.0f);
  Matrix4f rotation    = InitAxisRotation(0.0f, rot_counter, 0.0f);
  Matrix4f transform   = MatrixMul(projection, MatrixMul(translation, rotation));

  // Vector4f min_y_vert = VectorTransform(transform, CreateVector4f(-1, -1, 0, 1));
  // Vector4f mid_y_vert = VectorTransform(transform, CreateVector4f(0, 1, 0, 1));
  // Vector4f max_y_vert = VectorTransform(transform, CreateVector4f(1, -1, 0, 1));

  Vertex min_y_vert = VectorTransform(transform, CreateVertex(CreateVector4f(-1, -1, 0, 1), CreateVector4f(-1, -1, 0, 1)));
  Vertex mid_y_vert = VectorTransform(transform, CreateVertex(CreateVector4f(0, 1, 0, 1), CreateVector4f(-1, -1, 0, 1)));
  Vertex max_y_vert = VectorTransform(transform, CreateVertex(CreateVector4f(1, -1, 0, 1), CreateVector4f(-1, -1, 0, 1)));

  FillTriangle(max_y_vert, mid_y_vert, min_y_vert);
}

#endif
