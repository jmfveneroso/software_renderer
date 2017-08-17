#ifndef RENDER_H
#define RENDER_H

#include "player.h"
#include "level.h"
#include "bsp_tree.h"
#include "sector.h"
#include "visual_engine.h"
#include "alternative_render.h"

Point transform(Point p) {
  Point result;
  double x_dis = p.x - player.pos.x;
  double y_dis = p.y - player.pos.y;
  result.x = x_dis * cos(player.angle) + y_dis * sin(player.angle);
  result.y = y_dis * cos(player.angle) - x_dis * sin(player.angle);
  return result;
}

Point untransform(Point p) {
  Point result;

  // Rotation.
  result.x = p.x * cos(player.angle) - p.y * sin(player.angle);
  result.y = p.y * cos(player.angle) + p.x * sin(player.angle);

  // Translation.
  result.x += player.pos.x;
  result.y += player.pos.y;
  return result;
}

void DrawTransformedWall(Wall wall, int color) {
  Point origin = create_point(500, 150);
  Point p1 = transform(wall.p1);
  Point p2 = transform(wall.p2);

  DrawLine(p1.x + origin.x, p1.y + origin.y, p2.x + origin.x, p2.y + origin.y, color);
}

void DrawTransformedWalls() {
  Point origin;
  origin.x = 500;
  origin.y = 150;

  DrawCircle(origin.x, origin.y, 10, 10);
  DrawLine(origin.x, origin.y, origin.x + 40, origin.y, 10);

  for (int i = 0; i < 6; ++i) {
    DrawTransformedWall(wall[i], 2);
  }

  DrawLine(origin.x + pfrustum.distance, origin.y - pfrustum.size / 2, origin.x + pfrustum.distance, origin.y + pfrustum.size / 2, 10);
  FillRect(0, 301, 1000, 600, 11);
}

void DrawSectors() {
  for (int i = first_sector; i < num_sectors; ++i) { 
    WallList* cur_wall = sectors[i]->walls;
    while (cur_wall != NULL) {
      DrawTransformedWall(cur_wall->wall, 10);
      cur_wall = cur_wall->next;
    }
  }
}

void Draw3dWall(Wall* wall) {
  Point p1 = transform(wall->p1);
  Point p2 = transform(wall->p2);

  if (p1.x < 40 && p2.x < 40) return;

  Point pfrustum_top;
  pfrustum_top.x = pfrustum.distance;
  pfrustum_top.y = 30;
  Point pfrustum_bot;
  pfrustum_bot.x = pfrustum.distance;
  pfrustum_bot.y = -30;
  if (p1.x < 40)
    p1 = intersection(pfrustum_top, pfrustum_bot, p1, p2);
  if (p2.x < 40)
    p2 = intersection(pfrustum_top, pfrustum_bot, p1, p2);

  double left =  (double) pfrustum.distance * p1.y / p1.x; // p_y can vary from -x to +x. Where x = pfrustum_size / 2.
  double right = (double) pfrustum.distance * p2.y / p2.x; // p_y can vary from -x to +x. Where x = pfrustum_size / 2.

  if (p1.x - pfrustum.distance < 0) left = (p2.y < p1.y) ? (pfrustum.size / 2) : -(pfrustum.size / 2);
  if (p2.x - pfrustum.distance < 0) right = (p1.y < p2.y) ? (pfrustum.size / 2) : -(pfrustum.size / 2);

  if (fabs(right - left) > 10000) return;
  if (left > right) { 
    double tmp = right;
    right = left;
    left = tmp;
  }

  int lft_col = (1000 / (double) pfrustum.size) * (pfrustum.size / 2.0f + left);
  int rgt_col = (1000 / (double) pfrustum.size) * (pfrustum.size / 2.0f + right);

  for (int i = 0; i < rgt_col - lft_col; ++i) {
    Point origin; origin.x = 0; origin.y = 0;
    Point pfrustum_intersect;
    pfrustum_intersect.x = pfrustum.distance;
    pfrustum_intersect.y = left + i * ((double) fabs(right - left) / abs(rgt_col - lft_col));
    Point intersection_point = intersection(origin, pfrustum_intersect, p1, p2);
    double texture_x = vec_norm(vec_sub(intersection_point, transform(wall->p1)));

    if (intersection_point.x < pfrustum.distance) continue;
    double cos_p = intersection_point.x / vec_norm(intersection_point);
    if ((cos_p < 0.75 || cos_p == 1)) continue;

    // proj[i] = get_floor_proj_from_pos(points[i].x, points[i].y, sector->heights[i]);

    double projection_top = (wall->height - player.height) * pfrustum.distance / intersection_point.x;
    double projection_bottom = (0 - player.height) * pfrustum.distance / intersection_point.x;
    double projection_height = projection_top - projection_bottom;

    int window_mid = 450 + player.v_angle;

    int classification = classify_point(*wall, player.pos);
    if (classification == 1) {
      DrawWall(lft_col + i, window_mid - projection_top, window_mid - projection_bottom, wall->color, texture_x, wall->height, projection_height);
      ReserveCanvasWall(lft_col + i, window_mid - projection_top, window_mid - projection_bottom, WALL_FRONT_FACE);
    } else if (classification == -1) {
      ReserveCanvasWall(lft_col + i, window_mid - projection_top, window_mid - projection_bottom, WALL_BACK_FACE);
    }
  }
}

Point get_floor_pos_from_proj(double proj_x, double proj_y, double floor_height) {
  double numerator = proj_y - 450 - player.v_angle - player.height + floor_height;
  double denominator = player.height - floor_height;
  double total = 1 + numerator / denominator;
  double floor_x = pfrustum.distance / total;
  double floor_y = (proj_x * (pfrustum.size / 1000.0f) - (pfrustum.size / 2.0f)) * (floor_x / (double) pfrustum.distance);
  return create_point(floor_x, floor_y);
}

Point get_floor_proj_from_pos(double floor_x, double floor_y, double floor_height) {
  double proj_x = (1000 / (double) pfrustum.size) * ((pfrustum.size / 2.0f) + (floor_y / floor_x) * pfrustum.distance);
  double proj_y = 450 + player.v_angle + player.height - floor_height - ((floor_x - pfrustum.distance) * (player.height - floor_height)) / floor_x;
  return create_point(proj_x, proj_y);
}

Point uv_map(Point3 v1, Point3 v2, Point3 v3, Point u_vec, Point v_vec) {
  double s;
  if (abs(v2.z) < 0.0001f && abs(v1.z) < 0.0001f) {
    s = (v3.y / (v1.y + 0.0001)) - (v2.y / (v1.y + 0.0001)) * (v3.x / (v2.x + 0.0001));
    s /= 1 - (v2.y / (v1.y + 0.0001)) * (v1.x / (v2.x + 0.0001));
  } else {
    s = (v3.z / (v1.z + 0.0001)) - (v2.z / (v1.z + 0.0001)) * (v3.x / (v2.x + 0.0001));
    s /= 1 - (v2.z / (v1.z + 0.0001)) * (v1.x / (v2.x + 0.0001));
  }

  double t = (v3.x - v1.x * s) / (v2.x + 0.0001);

  Point uv_point; 
  uv_point.x = s * u_vec.x + t * v_vec.x;
  uv_point.y = s * u_vec.y + t * v_vec.y;
  return uv_point; 
}

bool PointInTriangle(Point v, Point v1, Point v2, Point v3) {
  // Cramer's rule.
  double det   = (v2.x - v1.x) * (v3.y - v1.y) - (v3.x - v1.x) * (v2.y - v1.y);
  double det_s = (v.x  - v1.x) * (v3.y - v1.y) - (v3.x - v1.x) * (v.y  - v1.y);
  double det_t = (v2.x - v1.x) * (v.y  - v1.y) - (v.x  - v1.x) * (v2.y - v1.y);

  double s = det_s / det;
  double t = det_t / det;
  return (s > 0.0f && t > 0.0f && s + t < 1.0f);
}

float sign(Point p1, Point p2, Point p3) {
  return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool PointInTri(Point pt, Point v1, Point v2, Point v3) {
  bool b1, b2, b3;

  b1 = sign(pt, v1, v2) < 0.0f;
  b2 = sign(pt, v2, v3) < 0.0f;
  b3 = sign(pt, v3, v1) < 0.0f;

  return ((b1 == b2) && (b2 == b3));
}

Point GetTexturePoint(Point3 points[3], int x, int y, double* distance) {
  Point3 a = create_point3(0, 0, player.height);

  double view_y = x * (pfrustum.size / 1000.0f) - (pfrustum.size / 2.0f);
  double view_z = player.height + player.v_angle + 450 - y;

  Point3 b = create_point3(pfrustum.distance, view_y, view_z);
  Point3 intersection = PlaneIntersection(a, b, points[0], points[1], points[2]);
  
  Point normalized;
  Point u_vec; u_vec.x = points[1].u - points[0].u; u_vec.y = points[1].v - points[0].v;
  Point v_vec; v_vec.x = points[2].u - points[0].u; v_vec.y = points[2].v - points[0].v;
  normalized = uv_map(vec3_sub(points[1], points[0]), vec3_sub(points[2], points[0]), vec3_sub(intersection, points[0]), u_vec, v_vec);

  *distance = vec3_norm(vec3_sub(intersection, a));
  return normalized;
}

void Draw3dTriangle(Point3 points[3], bool uv_mapping) {
  int min_x = 999999; int min_y = 999999;
  int max_x = -1;     int max_y = -1;

  int window_bottom = 600 + player.v_angle;
  Point proj[3];
  int i = 0;
  for (; i < 3; ++i) {
    proj[i] = get_floor_proj_from_pos(points[i].x, points[i].y, points[i].z);

    if (proj[i].x < min_x) min_x = proj[i].x;
    if (proj[i].y < min_y) min_y = proj[i].y;
    if (proj[i].x > max_x) max_x = proj[i].x;
    if (proj[i].y > max_y) max_y = proj[i].y;
  
    if (points[0].x < pfrustum.distance) return;
    if (points[1].x < pfrustum.distance) return;
    if (points[2].x < pfrustum.distance) return;
  }

  if (min_x < 0)   min_x = 0;
  if (min_y < 0)   min_y = 0;
  if (max_x > 999) max_x = 999;
  if (max_y > 599) max_y = 599;

  Point v1 = proj[0];
  Point v2 = proj[1];
  Point v3 = proj[2];
  // double det = (v2.x - v1.x) * (v3.y - v1.y) - (v3.x - v1.x) * (v2.y - v1.y);

  int size = 8;
  int x_squares = (max_x - min_x) / size;
  int y_squares = (max_y - min_y) / size;
  for (int y = 0; y <= y_squares; ++y) {
    for (int x = 0; x <= x_squares; ++x) {
      if (y == y_squares && (max_y - min_y) % size == 0) continue;
      if (x == x_squares && (max_x - min_x) % size == 0) continue;

      int size_x = size;
      int size_y = size;
      if (y == y_squares) size_y = (max_y - min_y) % size;
      if (x == x_squares) size_x = (max_x - min_x) % size;

      double distance1, distance2, distance3;
      Point uv1 = GetTexturePoint(points, min_x + x * size, min_y + y * size, &distance1);
      Point uv2 = GetTexturePoint(points, min_x + x * size + size_x - 1, min_y + y * size, &distance2);
      Point uv3 = GetTexturePoint(points, min_x + x * size, min_y + y * size + size_y - 1, &distance3);
      // Point uv1 = create_point(0, 0);
      // Point uv2 = create_point(0, 30);
      // Point uv3 = create_point(30, 30);

      double u = uv1.x;
      double v = uv1.y;
      double step_x = (uv2.x - uv1.x) / (double) size_x;
      double step_y = (uv2.y - uv1.y) / (double) size_y;
      double step_x2 = (uv3.x - uv1.x) / (double) size_x;
      double step_y2 = (uv3.y - uv1.y) / (double) size_y;
      double step_distance = (distance2 - distance1) / (double) size_x;
      double step_distance2 = (distance3 - distance1) / (double) size_y;
      double distance = distance1;

      int start_x = min_x + x * size;
      int start_y = min_y + y * size;
      for (int j = 0; j < size_y; ++j) {
        double old_u = u;
        double old_v = v;
        double old_distance = distance;
        for (int i = 0; i < size_x; ++i) {
          u += step_x;
          v += step_y;
          distance += step_distance;
          if (!PointInTri(create_point(start_x + i + 1, start_y + j + 1), proj[0], proj[1], proj[2])) continue;
          if (u > 0 && v > 0) {
            int tex_x = (int) u % textures[1].width;
            int tex_y = (int) v % textures[1].height;
            int base = tex_y * textures[1].width * 3 + tex_x * 3;
            unsigned char r = textures[1].rgb[base + 0];
            unsigned char g = textures[1].rgb[base + 1];
            unsigned char b = textures[1].rgb[base + 2];
            DrawPixelZ(start_x + i, start_y + j, r, g, b, distance);
          }
        }
        u = old_u; v = old_v; distance = old_distance;
        u += step_x2;
        v += step_y2;
        distance += step_distance2;
      }
    }
  }
}

void Draw3dFloor(Sector* sector) {
  Point points[3];

  WallList* cur_wall = sector->walls;
  points[0] = transform(cur_wall->wall.p1);
  points[1] = transform(cur_wall->wall.p2);

  cur_wall = cur_wall->next;
  points[2] = transform(cur_wall->wall.p2);

  Point3 points3[3];
  points3[0].x = points[0].x; points3[0].y = points[0].y; points3[0].z = sector->heights[0];
  points3[1].x = points[1].x; points3[1].y = points[1].y; points3[1].z = sector->heights[1];
  points3[2].x = points[2].x; points3[2].y = points[2].y; points3[2].z = sector->heights[2];
  Draw3dTriangle(points3, false);
}

Point rotateZ(Point p, int x, int y, double angle) {
  Point result;
  double x_dis = p.x - x;
  double y_dis = p.y - y;
  result.x = x_dis * cos(angle) + y_dis * sin(angle) + x;
  result.y = y_dis * cos(angle) - x_dis * sin(angle) + y;
  return result;
}

void Draw3dCube(Point p[4]) {
  // Front.
  Point3 v1[3];
  v1[0].x = p[0].x; v1[0].y = p[0].y;  v1[0].z = 2000; v1[0].u = 0;  v1[0].v = 0;
  v1[1].x = p[0].x; v1[1].y = p[0].y;  v1[1].z = 1500;  v1[1].u = 0;  v1[1].v = 30;
  v1[2].x = p[1].x; v1[2].y = p[1].y;  v1[2].z = 1500;  v1[2].u = 30; v1[2].v = 30;
  Point3 v2[3];
  v2[0].x = p[0].x; v2[0].y = p[0].y;  v2[0].z = 2000; v2[0].u = 0;  v2[0].v = 0;
  v2[1].x = p[1].x; v2[1].y = p[1].y;  v2[1].z = 2000; v2[1].u = 30; v2[1].v = 0;
  v2[2].x = p[1].x; v2[2].y = p[1].y;  v2[2].z = 1500;  v2[2].u = 30; v2[2].v = 30;

  // Below.
  Point3 v3[3];
  v3[0].x = p[0].x; v3[0].y = p[0].y;  v3[0].z = 1500; v3[0].u = 0;  v3[0].v = 0;
  v3[1].x = p[1].x; v3[1].y = p[1].y;  v3[1].z = 1500; v3[1].u = 0; v3[1].v = 30;
  v3[2].x = p[3].x; v3[2].y = p[3].y;  v3[2].z = 1500; v3[2].u = 30; v3[2].v = 30;

  Point3 v4[3];
  v4[0].x = p[0].x; v4[0].y = p[0].y;  v4[0].z = 1500; v4[0].u = 0;  v4[0].v = 0;
  v4[1].x = p[2].x; v4[1].y = p[2].y;  v4[1].z = 1500; v4[1].u = 30; v4[1].v = 0;
  v4[2].x = p[3].x; v4[2].y = p[3].y;  v4[2].z = 1500; v4[2].u = 30; v4[2].v = 30;

  // Left Side.
  Point3 v5[3];
  v5[0].x = p[0].x; v5[0].y = p[0].y;  v5[0].z = 2000; v5[0].u = 0;  v5[0].v = 0;
  v5[1].x = p[0].x; v5[1].y = p[0].y;  v5[1].z = 1500;  v5[1].u = 30; v5[1].v = 0;
  v5[2].x = p[2].x; v5[2].y = p[2].y;  v5[2].z = 1500;  v5[2].u = 30; v5[2].v = 30;

  Point3 v6[3];
  v6[0].x = p[0].x; v6[0].y = p[0].y;  v6[0].z = 2000; v6[0].u = 0;  v6[0].v = 0;
  v6[1].x = p[2].x; v6[1].y = p[2].y;  v6[1].z = 2000; v6[1].u = 0; v6[1].v = 30;
  v6[2].x = p[2].x; v6[2].y = p[2].y;  v6[2].z = 1500;  v6[2].u = 30; v6[2].v = 30;

  // Right Side.
  Point3 v7[3];
  v7[0].x = p[1].x; v7[0].y = p[1].y;  v7[0].z = 2000; v7[0].u = 0;  v7[0].v = 0;
  v7[1].x = p[1].x; v7[1].y = p[1].y;  v7[1].z = 1500;  v7[1].u = 30; v7[1].v = 0;
  v7[2].x = p[3].x; v7[2].y = p[3].y;  v7[2].z = 1500;  v7[2].u = 30; v7[2].v = 30;
                                                                                   
  Point3 v8[3];                                                                    
  v8[0].x = p[1].x; v8[0].y = p[1].y;  v8[0].z = 2000; v8[0].u = 0;  v8[0].v = 0;
  v8[1].x = p[3].x; v8[1].y = p[3].y;  v8[1].z = 2000; v8[1].u = 0;  v8[1].v = 30;
  v8[2].x = p[3].x; v8[2].y = p[3].y;  v8[2].z = 1500;  v8[2].u = 30; v8[2].v = 30;

  // Above.
  Point3 v9[3];
  v9[0].x = p[0].x; v9[0].y = p[0].y;  v9[0].z = 2000;    v9[0].u = 0;  v9[0].v = 0;
  v9[1].x = p[1].x; v9[1].y = p[1].y;  v9[1].z = 2000;    v9[1].u = 0;  v9[1].v = 30;
  v9[2].x = p[3].x; v9[2].y = p[3].y;  v9[2].z = 2000;    v9[2].u = 30; v9[2].v = 30;
                                                                                      
  Point3 v10[3];                                                                      
  v10[0].x = p[0].x; v10[0].y = p[0].y;  v10[0].z = 2000; v10[0].u = 0;  v10[0].v = 0;
  v10[1].x = p[2].x; v10[1].y = p[2].y;  v10[1].z = 2000; v10[1].u = 30; v10[1].v = 0;
  v10[2].x = p[3].x; v10[2].y = p[3].y;  v10[2].z = 2000; v10[2].u = 30; v10[2].v = 30;

  // Behind.
  Point3 v11[3];
  v11[0].x = p[2].x; v11[0].y = p[2].y;  v11[0].z = 2000; v11[0].u = 0;  v11[0].v = 0;
  v11[1].x = p[2].x; v11[1].y = p[2].y;  v11[1].z = 1500;  v11[1].u = 0;  v11[1].v = 30;
  v11[2].x = p[3].x; v11[2].y = p[3].y;  v11[2].z = 1500;  v11[2].u = 30; v11[2].v = 30;
  Point3 v12[3];                                                                      
  v12[0].x = p[2].x; v12[0].y = p[2].y;  v12[0].z = 2000; v12[0].u = 0;  v12[0].v = 0;
  v12[1].x = p[3].x; v12[1].y = p[3].y;  v12[1].z = 2000; v12[1].u = 30; v12[1].v = 0;
  v12[2].x = p[3].x; v12[2].y = p[3].y;  v12[2].z = 1500;  v12[2].u = 30; v12[2].v = 30;

  Draw3dTriangle(v1, true);
  Draw3dTriangle(v2, true);
  Draw3dTriangle(v3, true);
  Draw3dTriangle(v4, true);
  Draw3dTriangle(v5, true);
  Draw3dTriangle(v6, true);
  Draw3dTriangle(v7, true);
  Draw3dTriangle(v8, true);
  Draw3dTriangle(v9, true);
  Draw3dTriangle(v10, true);
  Draw3dTriangle(v11, true);
  Draw3dTriangle(v12, true);
}

void DrawCubes() {
  static double angle = 0.0f;
  // angle += 0.01f;

  
  Point p[4];

  ResetZBuffer();
  Point origin; origin.x = 100; origin.y = 100;
  for (int i = 0; i < 1; i++) {
    for (int j = 0; j < 1; j++) {
      origin.x = 100 + i * 150;
      origin.y = 100 + j * 150;

      p[0] = transform(rotateZ(create_point(origin.x - 20, origin.y - 20), origin.x, origin.y, angle));
      p[1] = transform(rotateZ(create_point(origin.x - 20, origin.y + 20000), origin.x, origin.y, angle));
      p[2] = transform(rotateZ(create_point(origin.x + 20000, origin.y - 20), origin.x, origin.y, angle));
      p[3] = transform(rotateZ(create_point(origin.x + 20000, origin.y + 20000), origin.x, origin.y, angle));
      Draw3dCube(p);

    }
  }
}

void DrawIfVisible(Wall* wall) {
  Point wall_p1 = transform(wall->p1);
  Point wall_p2 = transform(wall->p2);

  double cos_p1 = wall_p1.x / vec_norm(wall_p1);
  double cos_p2 = wall_p2.x / vec_norm(wall_p2);
  if ((cos_p1 > 0.75 && cos_p1 < 1) || (cos_p2 > 0.75 && cos_p2 < 1)) {
    Draw3dWall(wall);
  }  
}

void RenderBsp(BSPNode* node) {
  if (node == NULL) return;
  if (node->sector != -1) return;
  if (node->splitter == NULL) return;

  int classification = classify_point(*node->splitter, player.pos);
  if (classification == 1) { // Front.
    RenderBsp(node->front);
    DrawIfVisible(node->splitter);
    RenderBsp(node->back);
  } else if (classification == -1) { // Back.
    RenderBsp(node->back);
    DrawIfVisible(node->splitter);
    RenderBsp(node->front);
  } else {
    RenderBsp(node->front);
    RenderBsp(node->back);
  }
}

void RenderFloors(BSPNode* node) {
  if (node == NULL) return;
  if (node->sector != -1) {
    // Draw3dSector(sectors[node->sector]);
    // FloodFill3dSector(sectors[node->sector]);
    Draw3dFloor(sectors[node->sector]);
    return;
  }

  int classification = classify_point(*node->splitter, player.pos);
  if (classification == -1) { // Back.
    RenderFloors(node->back);
    RenderFloors(node->front);
  } else { // Front or inside.
    RenderFloors(node->front);
    RenderFloors(node->back);
  }
}

WallStack wall_stack;
void InitRenderer() {
  wall_stack.top = -1;
  for (int i = 0; i < NUM_WALLS; ++i) push(&wall_stack, &wall[i]);
  root = create_bsp_tree(&wall_stack, NUM_WALLS);
  create_sectors(&wall_stack);
}

void Render() {
  CleanCanvas();
  ClearProjectionScreen();

  // Screen divisions.
  // DrawLine(0, 300, 1000, 300, 10);

  // DrawSectors();
  // DrawTransformedWalls();
  // RenderBsp(root);

  // RenderFloors(root);
  // DrawCubes();
  // DrawTexture();
  DrawTriangle();
}

#endif
