#ifndef VERTEX_H
#define VERTEX_H

struct Vertex {
  Vector4f pos;
  Vector4f color;
  Vector4f tex_coords;
  Vector4f normal;
};

Vertex CreateVertex(Vector4f pos, Vector4f normal, Vector4f tex_coords) {
  Vertex v;
  v.pos = pos;
  // v.color = color;
  v.tex_coords = tex_coords;
  v.normal = normal;
  return v;
}

float GetComponent(Vertex v, int index) {
  switch (index) {
    case 1: return v.pos.x;
    case 2: return v.pos.y;
    case 3: return v.pos.z;
    case 4: return v.pos.w;
    default: return 0.0f;
  }
}

Vertex LerpVertex(Vertex a, Vertex b, float lerp_amt) {
  return CreateVertex(
    LerpVector(a.pos, b.pos, lerp_amt),
    LerpVector(a.normal, b.normal, lerp_amt),
    LerpVector(a.tex_coords, b.tex_coords, lerp_amt)
  );
}

bool IsInsideViewFrustum(Vertex v) {
  return fabs(v.pos.x) <= fabs(v.pos.w) &&
         fabs(v.pos.y) <= fabs(v.pos.w) &&
         fabs(v.pos.z) <= fabs(v.pos.w);
}


#endif
