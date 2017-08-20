#ifndef EDGE_H
#define EDGE_H

#include "gradients.h"

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
  float depth;
  float depth_step;
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

  e.depth = gradients.depth[min_y_vert_index] + gradients.depth_x_step * x_prestep + gradients.depth_y_step  * y_prestep;
  e.depth_step = gradients.depth_y_step + gradients.depth_x_step * e.x_step;
  return e;
}

void EdgeStep(Edge* e) {
  e->x += e->x_step;
  e->color = VectorAdd(e->color, e->color_step);
  e->tex_x += e->tex_x_step;
  e->tex_y += e->tex_y_step;
  e->one_over_z += e->one_over_z_step;
  e->depth += e->depth_step;
}

#endif
