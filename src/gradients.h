#ifndef GRADIENTS_H
#define GRADIENTS_H

#include "vertex.h"

struct Gradients {
  Vector4f colors[3];
  Vector4f color_x_step;
  Vector4f color_y_step;

  float tex_x[3];
  float tex_y[3];
  float one_over_z[3];
  float depth[3];
  float light_amt[3];

  float tex_xx_step;
  float tex_xy_step;
  float tex_yx_step;
  float tex_yy_step;

  float one_over_zx_step;
  float one_over_zy_step;

  float depth_x_step;
  float depth_y_step;

  float light_x_step;
  float light_y_step;
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

float Saturate(float val) {
  if (val < 0.0f) return 0.0f;
  if (val > 1.0f) return 0.0f;
  return val;
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

  g.depth[0] = min_y_vert.pos.z;
  g.depth[1] = mid_y_vert.pos.z;
  g.depth[2] = max_y_vert.pos.z;

  g.tex_x[0] = min_y_vert.tex_coords.x * g.one_over_z[0];
  g.tex_x[1] = mid_y_vert.tex_coords.x * g.one_over_z[1];
  g.tex_x[2] = max_y_vert.tex_coords.x * g.one_over_z[2];

  g.tex_y[0] = min_y_vert.tex_coords.y * g.one_over_z[0];
  g.tex_y[1] = mid_y_vert.tex_coords.y * g.one_over_z[1];
  g.tex_y[2] = max_y_vert.tex_coords.y * g.one_over_z[2];

  Vector4f light_dir = CreateVector4f(0, 0, 1, 0);
  g.light_amt[0] = Saturate(VectorDot(min_y_vert.normal, light_dir)) * 0.9f + 0.1f;
  g.light_amt[1] = Saturate(VectorDot(mid_y_vert.normal, light_dir)) * 0.9f + 0.1f;
  g.light_amt[2] = Saturate(VectorDot(max_y_vert.normal, light_dir)) * 0.9f + 0.1f;

  g.tex_xx_step = CalculateXStep(g.tex_x, min_y_vert, mid_y_vert, max_y_vert, one_over_dx); 
  g.tex_xy_step = CalculateYStep(g.tex_x, min_y_vert, mid_y_vert, max_y_vert, one_over_dy);
  g.tex_yx_step = CalculateXStep(g.tex_y, min_y_vert, mid_y_vert, max_y_vert, one_over_dx); 
  g.tex_yy_step = CalculateYStep(g.tex_y, min_y_vert, mid_y_vert, max_y_vert, one_over_dy); 
  g.one_over_zx_step = CalculateXStep(g.one_over_z, min_y_vert, mid_y_vert, max_y_vert, one_over_dx); 
  g.one_over_zy_step = CalculateYStep(g.one_over_z, min_y_vert, mid_y_vert, max_y_vert, one_over_dy); 
  g.depth_x_step = CalculateXStep(g.depth, min_y_vert, mid_y_vert, max_y_vert, one_over_dx); 
  g.depth_y_step = CalculateYStep(g.depth, min_y_vert, mid_y_vert, max_y_vert, one_over_dy); 
  g.light_x_step = CalculateXStep(g.light_amt, min_y_vert, mid_y_vert, max_y_vert, one_over_dx); 
  g.light_y_step = CalculateYStep(g.light_amt, min_y_vert, mid_y_vert, max_y_vert, one_over_dy); 

  return g;
}

#endif
