#ifndef VECTOR_H
#define VECTOR_H

struct Vector4f {
  float x, y, z, w;
};

Vector4f CreateVector4f(float x, float y, float z, float w) {
  Vector4f v; 
  v.x = x; v.y = y;
  v.z = z; v.w = w;
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

#endif
