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
  Vector4f tex_coords;
};

Vertex CreateVertex(Vector4f pos, Vector4f color, Vector4f tex_coords) {
  Vertex v;
  v.pos = pos;
  v.color = color;
  v.tex_coords = tex_coords;
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

Vector4f LerpVector(Vector4f origin, Vector4f dest, float lerp_factor) {
  return VectorAdd(VectorScalarMul(VectorSub(dest, origin), lerp_factor), origin);
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
  return CreateVertex(CreateVector4f(x, y, z, w), v.color, v.tex_coords);
}

Vertex VectorPerspectiveDivide(Vertex r) {
  Vector4f v = r.pos;
  return CreateVertex(CreateVector4f(v.x / v.w, v.y / v.w, v.z / v.w, v.w), r.color, r.tex_coords);
}

//====================
// Gradients
//====================

struct Gradients {
  Vector4f colors[3];
  Vector4f color_x_step;
  Vector4f color_y_step;

  float tex_x[3];
  float tex_y[3];
  float one_over_z[3];

  float tex_xx_step;
  float tex_xy_step;
  float tex_yx_step;
  float tex_yy_step;

  float one_over_zx_step;
  float one_over_zy_step;
};

float CalculateXStep(float values[3], Vertex min_y_vert, Vertex mid_y_vert, Vertex max_y_vert, float one_over_dx) {
  float d_cx = ((values[1] - values[2]) * (min_y_vert.pos.y - max_y_vert.pos.y)) -
              ((values[0] - values[2]) * (mid_y_vert.pos.y - max_y_vert.pos.y));
  return d_cx * one_over_dx;
}

float CalculateYStep(float values[3], Vertex min_y_vert, Vertex mid_y_vert, Vertex max_y_vert, float one_over_dy) {
  float d_cy = ((values[1] - values[2]) * (min_y_vert.pos.x - max_y_vert.pos.x)) -
               ((values[0] - values[2]) * (mid_y_vert.pos.x - max_y_vert.pos.x));
  return d_cy * one_over_dy;
}

Gradients CreateGradients(Vertex min_y_vert, Vertex mid_y_vert, Vertex max_y_vert) {
  Gradients g;
  g.colors[0] = min_y_vert.color;
  g.colors[1] = mid_y_vert.color;
  g.colors[2] = max_y_vert.color;

  float one_over_dx = 1.0f / (float) 
    ((mid_y_vert.pos.x - max_y_vert.pos.x) * (min_y_vert.pos.y - max_y_vert.pos.y) -
    (min_y_vert.pos.x - max_y_vert.pos.x) * (mid_y_vert.pos.y - max_y_vert.pos.y));
                      
  float one_over_dy = -one_over_dx;

  Vector4f d_color_x = VectorSub(
    VectorScalarMul(VectorSub(g.colors[1], g.colors[2]), min_y_vert.pos.y - max_y_vert.pos.y),
    VectorScalarMul(VectorSub(g.colors[0], g.colors[2]), mid_y_vert.pos.y - max_y_vert.pos.y)
  );

  Vector4f d_color_y = VectorSub(
    VectorScalarMul(VectorSub(g.colors[1], g.colors[2]), min_y_vert.pos.x - max_y_vert.pos.x),
    VectorScalarMul(VectorSub(g.colors[0], g.colors[2]), mid_y_vert.pos.x - max_y_vert.pos.x)
  );

  g.color_x_step = VectorScalarMul(d_color_x, one_over_dx); 
  g.color_y_step = VectorScalarMul(d_color_y, one_over_dy); 

  // Texture.

  g.one_over_z[0] = 1.0f / min_y_vert.pos.w;
  g.one_over_z[1] = 1.0f / mid_y_vert.pos.w;
  g.one_over_z[2] = 1.0f / max_y_vert.pos.w;

  g.tex_x[0] = min_y_vert.tex_coords.x * g.one_over_z[0];
  g.tex_x[1] = mid_y_vert.tex_coords.x * g.one_over_z[1];
  g.tex_x[2] = max_y_vert.tex_coords.x * g.one_over_z[2];

  g.tex_y[0] = min_y_vert.tex_coords.y * g.one_over_z[0];
  g.tex_y[1] = mid_y_vert.tex_coords.y * g.one_over_z[1];
  g.tex_y[2] = max_y_vert.tex_coords.y * g.one_over_z[2];

  g.tex_xx_step = CalculateXStep(g.tex_x, min_y_vert, mid_y_vert, max_y_vert, one_over_dx); 
  g.tex_xy_step = CalculateYStep(g.tex_x, min_y_vert, mid_y_vert, max_y_vert, one_over_dy);
  g.tex_yx_step = CalculateXStep(g.tex_y, min_y_vert, mid_y_vert, max_y_vert, one_over_dx); 
  g.tex_yy_step = CalculateYStep(g.tex_y, min_y_vert, mid_y_vert, max_y_vert, one_over_dy); 
  g.one_over_zx_step = CalculateXStep(g.one_over_z, min_y_vert, mid_y_vert, max_y_vert, one_over_dx); 
  g.one_over_zy_step = CalculateYStep(g.one_over_z, min_y_vert, mid_y_vert, max_y_vert, one_over_dy); 

  return g;
}

//====================
// Edge
//====================

struct Edge {
  float x;
  float x_step;
  int y_start;  
  int y_end;
  Vector4f color;
  Vector4f color_step;
  float tex_x;
  float tex_y;
  float tex_x_step;
  float tex_y_step;
  float one_over_z;
  float one_over_z_step;
};

Edge CreateEdge(Gradients gradients, Vertex min_y_vert, Vertex max_y_vert, int min_y_vert_index) {
  Edge e;
  e.y_start = (int) ceil(min_y_vert.pos.y);
  e.y_end   = (int) ceil(max_y_vert.pos.y);

  float y_dist = max_y_vert.pos.y - min_y_vert.pos.y; 
  float x_dist = max_y_vert.pos.x - min_y_vert.pos.x; 

  float y_prestep = e.y_start - min_y_vert.pos.y;
  e.x_step = (float) x_dist / (float) y_dist;
  e.x = (float) min_y_vert.pos.x + y_prestep * e.x_step;

  float x_prestep = e.x - min_y_vert.pos.x;
  e.color = VectorAdd(
    VectorAdd(
      gradients.colors[min_y_vert_index], VectorScalarMul(gradients.color_y_step, y_prestep)
    ),
    VectorScalarMul(gradients.color_x_step, x_prestep)
  );

  e.color_step = VectorAdd(gradients.color_y_step, VectorScalarMul(gradients.color_x_step, e.x_step));

  // Texture.
  e.tex_x = gradients.tex_x[min_y_vert_index] + gradients.tex_xx_step * x_prestep + gradients.tex_xy_step * y_prestep;
  e.tex_x_step = gradients.tex_xy_step + gradients.tex_xx_step  * e.x_step;
  e.tex_y = gradients.tex_y[min_y_vert_index] + gradients.tex_yx_step * x_prestep + gradients.tex_yy_step * y_prestep;
  e.tex_y_step = gradients.tex_yy_step + gradients.tex_yx_step  * e.x_step;

  // Perspective correct interpolation.
  e.one_over_z = gradients.one_over_z[min_y_vert_index] + gradients.one_over_zx_step * x_prestep + gradients.one_over_zy_step  * y_prestep;
  e.one_over_z_step = gradients.one_over_zy_step + gradients.one_over_zx_step * e.x_step;

  return e;
}

void EdgeStep(Edge* e) {
  e->x += e->x_step;
  e->color = VectorAdd(e->color, e->color_step);
  e->tex_x += e->tex_x_step;
  e->tex_y += e->tex_y_step;
  e->one_over_z += e->one_over_z_step;
}

void DrawScanLine(Gradients gradients, Edge* left, Edge* right, int j, Texture texture) {
  int x_min = ceil(left->x);
  int x_max = ceil(right->x);
  float x_prestep = x_min - left->x;

  float x_dist = right->x - left->x;
  float tex_xx_step = (right->tex_x - left->tex_x) / x_dist;
  float tex_yx_step = (right->tex_y - left->tex_y) / x_dist;
  float one_over_zx_step = (right->one_over_z - left->one_over_z) / x_dist;

  Vector4f color = VectorAdd(left->color, VectorScalarMul(gradients.color_x_step, x_prestep));

  float tex_x = left->tex_x + tex_xx_step * x_prestep;
  float tex_y = left->tex_y + tex_yx_step * x_prestep;
  float one_over_z = left->one_over_z + one_over_zx_step * x_prestep;

  for (int i = x_min; i < x_max; i++) {
    // unsigned char r = (unsigned char) (color.x * 255 + 0.5f);
    // unsigned char g = (unsigned char) (color.y * 255 + 0.5f);
    // unsigned char b = (unsigned char) (color.z * 255 + 0.5f);

    // Texture.
    float z = 1.0f / one_over_z;
    int src_x = (int) ((tex_x * z) * (texture.width  - 1) + 0.5f);
    int src_y = (int) ((tex_y * z) * (texture.height - 1) + 0.5f);
   
    int base = (src_y * texture.width + src_x) * 3;
    unsigned char r = texture.rgb[base];
    unsigned char g = texture.rgb[base + 1];
    unsigned char b = texture.rgb[base + 2];
 
    DrawPixel(i, j, r, g, b);

    color = VectorAdd(color, gradients.color_x_step);
    tex_x += tex_xx_step;
    tex_y += tex_yx_step;
    one_over_z += one_over_zx_step;
  }
}

void ScanEdges(Gradients gradients, Edge* a, Edge* b, bool handedness, Texture texture) {
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

void ScanTriangle(Vertex min_y_vert, Vertex mid_y_vert, Vertex max_y_vert, bool handedness, Texture texture) {
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

void FillTriangle(Vertex v1, Vertex v2, Vertex v3, Texture texture) {
  Matrix4f screen_space_transform = InitScreenSpaceTransform(500, 300);
  v1 = VectorTransform(screen_space_transform, v1);
  v2 = VectorTransform(screen_space_transform, v2);
  v3 = VectorTransform(screen_space_transform, v3);
  Vertex min_y_vert = VectorPerspectiveDivide(v1);
  Vertex mid_y_vert = VectorPerspectiveDivide(v2);
  Vertex max_y_vert = VectorPerspectiveDivide(v3);

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

  float area = TriangleAreaTimesTwo(min_y_vert, max_y_vert, mid_y_vert);
  ScanTriangle(min_y_vert, mid_y_vert, max_y_vert, area >= 0, texture);
}

void DrawTriangle() {
  Matrix4f projection = InitPerspective(1.22173, 1000.0f / 600.0f, 0.1f, 1000.0f);
  static float rot_counter = 0.0f;
  rot_counter += 0.03;

  Matrix4f translation = InitTranslation(0.0f, 0.0f, 5.0f);
  Matrix4f rotation    = InitAxisRotation(rot_counter, rot_counter, rot_counter);
  Matrix4f transform   = MatrixMul(projection, MatrixMul(translation, rotation));

  Vertex min_y_vert = VectorTransform(transform, CreateVertex(CreateVector4f(-1, -1, 0, 1), CreateVector4f(1, 0, 0, 0), CreateVector4f(0, 0, 0, 0)));
  Vertex mid_y_vert = VectorTransform(transform, CreateVertex(CreateVector4f(0, 1, 0, 1), CreateVector4f(0, 1, 0, 0), CreateVector4f(0.5f, 1.0f, 0, 0)));
  Vertex max_y_vert = VectorTransform(transform, CreateVertex(CreateVector4f(1, -1, 0, 1), CreateVector4f(0, 0, 1, 0), CreateVector4f(1.0f, 0, 0, 0)));

  FillTriangle(max_y_vert, mid_y_vert, min_y_vert, textures[1]);
}

#endif
