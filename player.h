#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
  int distance;
  int size;
} Frustum;

typedef struct {
  Point pos;
  double angle;
  double v_angle;
  double height;
} Player;

Player player;
Frustum pfrustum;

#endif
