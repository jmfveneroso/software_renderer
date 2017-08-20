#ifndef BSP_TREE_H
#define BSP_TREE_H

#include "level.h"
#include "vector.h"

typedef struct {
  Wall walls[1000];
  int top;
} WallStack;

typedef struct BSPNode {
  int sector;
  Wall* splitter;
  struct BSPNode* front;
  struct BSPNode* back;
  struct BSPNode* parent;
} BSPNode;

BSPNode* root;

void push(WallStack* wall_stack, Wall* wall) {
  wall_stack->walls[++wall_stack->top] = *wall;
}

int classify_point(Wall wall, Point p) {
  Point direction;
  direction.x = p.x - wall.p1.x;
  direction.y = p.y - wall.p1.y;
  double cos_angle = dot_product(wall.normal, direction);

  if (fabs(cos_angle) < 0.0001) return 0;

  return (cos_angle > 0) ? 1 : -1;
}

int classify_polygon(Wall* wall1, Wall* wall2) {
  int v1, v2;
  v1 = classify_point(*wall1, wall2->p1);
  v2 = classify_point(*wall1, wall2->p2);
  if (v1 + v2 > 0) return 1;
  if (v1 + v2 < 0) return -1;
  return 0;
}

Wall* split_wall(Wall* wall_1, Wall* wall_2) {
  Point target = vec_sub(wall_2->p2, wall_2->p1);

  double numerator = dot_product(vec_sub(wall_1->p1, wall_2->p1), wall_1->normal);
  double denominator = dot_product(target, wall_1->normal);
  double s = numerator / denominator;

  Point intersection = vec_add(wall_2->p1, vec_scalar(s, target));
  Point end = wall_2->p2;
  wall_2->p2 = intersection;

  Wall* new_wall = (Wall*) malloc(sizeof(Wall));
  new_wall->normal = wall_2->normal;
  new_wall->color = rand() % 11;
  new_wall->p1 = intersection;
  new_wall->p2 = end;
  new_wall->height = wall_2->height;
  return new_wall;
}

BSPNode* create_bsp_tree(WallStack* wall_stack, int size) {
  if (size == 0) return NULL;

  WallStack front_stack;
  WallStack back_stack;
  front_stack.top = -1;
  back_stack.top = -1;

  BSPNode* node = (BSPNode*) malloc(sizeof(BSPNode));
  node->parent = NULL;
  node->sector = -1;
  node->splitter = &wall_stack->walls[wall_stack->top]; // The splitter should be properly selected.
  node->splitter->color = rand() % 11;
  for (int i = 1; i < size; ++i) {
    --wall_stack->top;
    Wall* wall = &wall_stack->walls[wall_stack->top];
    int type = classify_polygon(node->splitter, wall);
    if (type == 1) {
      push(&front_stack, wall);
    } else if (type == -1) {
      push(&back_stack, wall);
    } else {
      Wall* new_wall = split_wall(node->splitter, wall);
      if (classify_polygon(node->splitter, new_wall) == 1) {
        push(&front_stack, new_wall);
        push(&back_stack, wall);
      } else {
        push(&front_stack, wall);
        push(&back_stack, new_wall);
      }
    }
  }

  node->front = create_bsp_tree(&front_stack, front_stack.top + 1);
  if (node->front != NULL) node->front->parent = node;
  node->back = create_bsp_tree(&back_stack, back_stack.top + 1);
  if (node->back != NULL) node->back->parent = node;
  return node;
}

#endif
