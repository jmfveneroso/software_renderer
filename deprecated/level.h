#ifndef LEVEL_H
#define LEVEL_H

#include "vector.h"

#define NUM_WALLS 6

Wall wall[NUM_WALLS];
Wall split_walls[100];
Wall boundary_walls[4];
int wall_counter = 0;

Wall CreateWall(double x1, double y1, double x2, double y2, double height) {
  Wall wall_;
  wall_.p1.x = x1;
  wall_.p1.y = y1;
  wall_.p2.x = x2;
  wall_.p2.y = y2;
  wall_.height = height;
  wall_.normal = get_normal(wall_);
  return wall_;
}

void InsertWall(Wall wall_) {
  wall[wall_counter] = wall_;
  wall_counter++;
}

void CreateWalls() {
  // InsertWall(CreateWall(120, 250, 120, 350, 1500));
  // InsertWall(CreateWall(120, 350, 200, 300, 500));
  // InsertWall(CreateWall(200, 300, 120, 250, 200));
  // InsertWall(CreateWall(100, 100, 200, 100, 300));
  // InsertWall(CreateWall(200, 100, 200, 200, 1200));
  // InsertWall(CreateWall(200, 200, 100, 200, 1000));
  // InsertWall(CreateWall(100, 200, 100, 100, 100));

  InsertWall(CreateWall(80, 80, 120, 80, 200));
  wall[0].normal = create_point(0, -20);

  InsertWall(CreateWall(120, 80, 120, 120, 200));
  wall[1].normal = create_point(20, 0);
  InsertWall(CreateWall(90, 120, 120, 120, 200));
  Point p = wall[2].normal;
  printf("Normal %f %f\n", p.x, p.y);
  wall[2].normal = create_point(0, 20);
  InsertWall(CreateWall(80, 80, 90, 120, 200));
  p = wall[3].normal;
  printf("Normal %f %f\n", p.x, p.y);
  wall[3].normal = create_point(-19.402850, 4.850713);

  InsertWall(CreateWall(230, 250, 250, 50, 200));
  wall[4].height = 500;

  InsertWall(CreateWall(50, 250, 230, 250, 200));
  wall[5].height = 500;

  // InsertWall(CreateWall(50, 50, 250, 50, 200));
  // wall[5].normal = create_point(0, 20);
  // wall[5].height = 500;
  // wall[5].color = 11;

  boundary_walls[0] = CreateWall(0, 0, 300, 0, 0);
  boundary_walls[0].normal = create_point(0, 1);
  boundary_walls[1] = CreateWall(300, 0, 300, 300, 0);
  boundary_walls[1].normal = create_point(-1, 0);
  boundary_walls[2] = CreateWall(0, 300, 300, 300, 0);
  boundary_walls[2].normal = create_point(0, -1);
  boundary_walls[3] = CreateWall(0, 0, 0, 300, 0);
  boundary_walls[3].normal = create_point(1, 0);
}

#endif
