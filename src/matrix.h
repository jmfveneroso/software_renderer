#ifndef MATRIX_H
#define MATRIX_H

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
  mat.m[0][0] = half_width; mat.m[0][1] = 0;             mat.m[0][2] = 0; mat.m[0][3] = half_width - 0.5f;
  mat.m[1][0] = 0;          mat.m[1][1] = -half_height;  mat.m[1][2] = 0; mat.m[1][3] = half_height - 0.5f;
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

#endif
