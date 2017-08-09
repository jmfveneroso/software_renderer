#ifndef RENDER_H
#define RENDER_H

#include "player.h"
#include "level.h"
#include "bsp_tree.h"
#include "sector.h"
#include "visual_engine.h"

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
  for (int i = 0; i < num_sectors; ++i) { 
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

  int lft_col = (double) (1000 / pfrustum.size) * (pfrustum.size / 2 + left);
  int rgt_col = (double) (1000 / pfrustum.size) * (pfrustum.size / 2 + right);

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

void GetFloorHeight() {
}

void Draw3dSector(Sector* sector) {
  int min_x = 999999, max_x = -999999;

  WallList* cur_wall = sector->walls;
  while (cur_wall != NULL) {
    Point p1 = transform(cur_wall->wall.p1);
    Point p2 = transform(cur_wall->wall.p2);
    if (p1.x > max_x) max_x = p1.x;
    if (p1.x < min_x) min_x = p1.x;
    if (p2.x > max_x) max_x = p2.x;
    if (p2.x < min_x) min_x = p2.x;

    cur_wall = cur_wall->next;
  }

  if (max_x < pfrustum.distance) return;
  if (min_x < pfrustum.distance) min_x = pfrustum.distance;

  double floor_height = sector->height;
  if (player.height < floor_height) return;

  int proj_y_min = player.height - (pfrustum.distance / (double) min_x) * (player.height - floor_height);
  int proj_y_max = player.height - (pfrustum.distance / (double) max_x) * (player.height - floor_height);

  for (int proj_y = proj_y_min; proj_y <= proj_y_max; proj_y++) {
    double floor_x = pfrustum.distance * (player.height - floor_height) / ((double) player.height - (double) proj_y);
    // printf("floor_x %f\n", floor_x);
    Wall comparison_line = create_wall(floor_x, -1000, floor_x, 1000);

    double min_y = 999999;
    double max_y = -999999;
    WallList* cur_wall = sector->walls;
    while (cur_wall != NULL) {
      Wall transformed_wall;
      transformed_wall.p1 = transform(cur_wall->wall.p1);
      transformed_wall.p2 = transform(cur_wall->wall.p2);
      // printf("trans %f %f %f %f\n", transformed_wall.p1.x, transformed_wall.p1.y, transformed_wall.p2.x, transformed_wall.p2.y);
      Point origin;
      origin.x = 500;
      origin.y = 150;

      Point inter = segment_intersection(comparison_line, transformed_wall);
      // printf("trans %f %f %f %f\n", transformed_wall.p1.x, transformed_wall.p1.y, transformed_wall.p2.x, transformed_wall.p2.y);
      // printf("trans %f %f %f %f\n", comparison_line.p1.x, comparison_line.p1.y, comparison_line.p2.x, comparison_line.p2.y);
      // Point inter = intersection(comparison_line.p1, comparison_line.p2, transformed_wall.p1, transformed_wall.p2);
      if (!inter.null) {
        // DrawCircle(x11, origin.x + inter.x, origin.y + inter.y, 2, 10);
        // DrawLine(x11, origin.x + comparison_line.p1.x, origin.y + comparison_line.p1.y, origin.x + comparison_line.p2.x, origin.y + comparison_line.p2.y, 2); // Red.
        // DrawLine(x11, origin.x + transformed_wall.p1.x, origin.y + transformed_wall.p1.y, origin.x + transformed_wall.p2.x, origin.y + transformed_wall.p2.y, 3); // Red.
        // printf("inter %f %f\n", inter.x, inter.y);
        if (inter.y < min_y) min_y = inter.y;
        if (inter.y > max_y) max_y = inter.y;
      }
      cur_wall = cur_wall->next;
      // break;
    }

    if (min_y == 999999 || max_y == -999999) continue;

    // printf("minmax %f %f\n", min_y, max_y);
    double proj_x_min = (min_y / (double) floor_x) * pfrustum.distance;
    double proj_x_max = (max_y / (double) floor_x) * pfrustum.distance;
    int lft = (double) (1000 / pfrustum.size) * ((pfrustum.size / 2) + proj_x_min);
    int rgt = (double) (1000 / pfrustum.size) * ((pfrustum.size / 2) + proj_x_max);
     
    int actual_proj_y = proj_y - (player.height - 150) + 1;

    Point floor_p1 = untransform(create_point(floor_x, min_y));
    Point floor_p2 = untransform(create_point(floor_x, max_y));

    int window_bottom = 600 + player.v_angle;
    DrawFloor(window_bottom - actual_proj_y, lft, rgt, floor_p1, floor_p2, sector->color); // Red.
    ReserveCanvasFloor(window_bottom - actual_proj_y, lft, rgt, FLOOR);
    // DrawLine(x11, lft, 600 - actual_proj_y, rgt, 600 - actual_proj_y, 2); // Red.
  }
   
  // DrawLine(x11, 0, 600 - proj_y_min, 500, 600 - proj_y_min, 2); // Red.
  // DrawLine(x11, 0, 600 - proj_y_max, 500, 600 - proj_y_max, 3); // Yellow.
  // printf("min x: %d\n", min_x); 
  // printf("max x: %d\n", max_x); 
  // printf("proj min x: %d\n", proj_y_min); 
  // printf("proj max x: %d\n", proj_y_max); 

  // printf("min x: %d\n", min_x); 
  // printf("max x: %d\n", max_x); 
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
    Draw3dSector(sectors[node->sector]);
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
  create_sectors();
}

void Render() {
  CleanCanvas();
  ClearProjectionScreen();

  // Screen divisions.
  DrawLine(0, 300, 1000, 300, 10);

  DrawSectors();
  DrawTransformedWalls();
  // RenderBsp(root);

  RenderFloors(root);
  // DrawTexture();
}

#endif
