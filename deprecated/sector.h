#ifndef SECTOR_H
#define SECTOR_H

#include <assert.h>
#include "bsp_tree.h"

typedef struct WallList {
  Wall wall;
  int side;
  struct WallList* next;
  struct WallList* prev;
} WallList;

typedef struct {
  WallList* walls;
  double height;
  int color;
  double heights[3];
} Sector;

int first_sector;
int num_sectors;
Sector* sectors[20];

int num_tri_sectors;
Sector* tri_sectors[40];

WallStack* bla_stack;

Sector* new_sector() {
  Sector* s = (Sector*) malloc(sizeof(Sector));
  s->walls = NULL;
  s->height = 0;
  return s;
}

void print_sector(Sector* s) {
  printf("===begin sector\n");
  WallList* cur_wall = s->walls;
  while (cur_wall != NULL) {
    Wall* w = &cur_wall->wall;
    printf("  wall %f %f %f %f\n", w->p1.x, w->p1.y, w->p2.x, w->p2.y);
    cur_wall = cur_wall->next;
  }
  printf("===end sector\n");
}

Sector* copy_sector(Sector* s) {
  Sector* cpy = (Sector*) malloc(sizeof(Sector));
  cpy->height = s->height;
  cpy->walls = NULL;

  WallList* cur = NULL;
  for (WallList* w = s->walls; w != NULL; w = w->next) {
    WallList* new_wall = (WallList*) malloc(sizeof(WallList));
    new_wall->wall = w->wall;
    new_wall->side = w->side;
    new_wall->next = NULL;

    if (cur == NULL) {
      cpy->walls = new_wall;
    } else {
      cur->next = new_wall;
    }

    new_wall->prev = cur;
    cur = new_wall;
  }
  return cpy;
}

bool sector_has_point(Sector* sector, Point p) {
  WallList* temp = sector->walls;
  int counter = 0;
  while (temp != NULL) {
    if (temp == NULL) printf("lamama");
    counter++;
    int side = classify_point(temp->wall, p);
    // printf("classify x: %f y: %f\n", p.x, p.y);
    // printf("classify wall %f %f %f %f\n", temp->wall.p1.x, temp->wall.p1.y, temp->wall.p2.x, temp->wall.p2.y);
    // printf("side %d\n", side);
    if (side != temp->side && side != 0) return false;
    temp = temp->next;
  }
  return true;
}

bool sector_crop_walls(Sector* s, Wall* new_wall, int side) {
  // printf("GOLD==========\n");
  // print_sector(s);
  // printf("TEETH==========\n");
  int num_intersections = 0;
  Point intersections[2];
  Wall* w = new_wall;
  for (WallList* temp = s->walls; temp != NULL; temp = temp->next) {
    Wall* w = &temp->wall;

    Point inter = intersection(new_wall->p1, new_wall->p2, w->p1, w->p2);
    // printf("==========\n");
    // printf("star wall %f %f %f %f\n", new_wall->p1.x, new_wall->p1.y, new_wall->p2.x, new_wall->p2.y);
    // printf("butterfly wall %f %f %f %f\n", w->p1.x, w->p1.y, w->p2.x, w->p2.y);
    if (inter.null) continue;
    // printf("x: %f y: %f\n", inter.x, inter.y);

    if (!sector_has_point(s, inter)) continue;
    // printf("HAS POINT\n");

    int p1_pos = classify_point(*new_wall, w->p1);
    int p2_pos = classify_point(*new_wall, w->p2);

    if (p1_pos != side && p1_pos != 0)
      temp->wall.p1 = inter;
    else if (p2_pos != side && p2_pos != 0)
      temp->wall.p2 = inter;

    intersections[num_intersections] = inter;
    if (++num_intersections >= 2) {
      // printf("seus misterios\n");
      break;
    }
  }
 
  assert(num_intersections == 2);
  new_wall->p1 = intersections[0];
  new_wall->p2 = intersections[1];
  return true;
}

void sector_remove_unused_walls(Sector* s, Wall new_wall, int side) {
  for (WallList* temp = s->walls; temp != NULL; temp = temp->next) {
    Wall* w = &temp->wall;
    int actual_side = classify_polygon(&new_wall, w);
    if (actual_side == side || actual_side == 0) continue;
  
    if (temp->prev == NULL) s->walls = temp->next;
    else temp->prev->next = temp->next;

    if (temp->next != NULL) {
      temp->next->prev = temp->prev;
    }

    // TODO: free memory for temp wall.
  }
}

void sector_add_wall(Sector* s, Wall wall, int side, bool crop) {
  WallList* new_wall = (WallList*) malloc(sizeof(WallList));
  new_wall->wall = wall;
  new_wall->side = side;
  new_wall->prev = NULL;
  new_wall->next = NULL;

  if (s->walls == NULL) {
    s->walls = new_wall;
    return;
  }

  if (crop) {
    bool parallel = sector_crop_walls(s, &new_wall->wall, side);
    sector_remove_unused_walls(s, new_wall->wall, side);
    // if (parallel) return;
  }

  WallList* temp = s->walls;
  for (; temp != NULL; temp = temp->next)
    if (temp->next == NULL) break;

  temp->next = new_wall;
  new_wall->prev = temp;
}

void create_sectors_aux(BSPNode* node, Sector* s) {
  if (node == NULL) {
    // sectors[num_sectors] = s; 
    // num_sectors++;
    return;
  }

  // printf("SECTOR ");
  Wall* w = node->splitter;
  // printf(" %f %f %f %f\n", w->p1.x, w->p1.y, w->p2.x, w->p2.y);

  Sector* s_cpy_1 = copy_sector(s);
  Sector* s_cpy_2 = copy_sector(s);
  // WallList* cur_wall = s_cpy_1->walls;
  // while (cur_wall != NULL) {
  //   Wall* w = &cur_wall->wall;
  //   printf("W %f %f %f %f\n", w->p1.x, w->p1.y, w->p2.x, w->p2.y);
  //   cur_wall = cur_wall->next;
  // }
  // printf("\n");

  // Wall* w = node->splitter;
  // printf("Front %f %f %f %f\n", w->p1.x, w->p1.y, w->p2.x, w->p2.y);
  sector_add_wall(s_cpy_1, *node->splitter, 1, true); // Front.
  if (node->front == NULL) {
    BSPNode* sector_node = (BSPNode*) malloc(sizeof(BSPNode));
    sector_node->sector = num_sectors;
    sectors[num_sectors] = s_cpy_1; 
    // printf("Created sector %d\n", num_sectors);
    num_sectors++;
    node->front = sector_node;
  } else {
    create_sectors_aux(node->front, s_cpy_1);
  }

  // printf("Back %f %f %f %f\n", w->p1.x, w->p1.y, w->p2.x, w->p2.y);
  sector_add_wall(s_cpy_2, *node->splitter, -1, true); // Back.
  if (node->back == NULL) {
    BSPNode* sector_node = (BSPNode*) malloc(sizeof(BSPNode));
    sector_node->sector = num_sectors;
    sectors[num_sectors] = s_cpy_2; 
    // printf("Created sector %d\n", num_sectors);
    num_sectors++;
    node->back = sector_node;
  } else {
    create_sectors_aux(node->back, s_cpy_2);
  }
}

int sector_num_walls(Sector* sector) {
  int num_walls = 0;
  WallList* temp = sector->walls;
  while (temp != NULL) {
    ++num_walls;
    temp = temp->next;
  }
  return num_walls;
}





void create_triangular_sectors(BSPNode* node, Point point_array[], int array_size, int second_point) {
  printf("bla %d %d\n", array_size, second_point);
  Wall wall1 = create_wall_p(point_array[0], point_array[second_point]);
  Wall wall2 = create_wall_p(point_array[second_point], point_array[second_point + 1]);

  Wall* wall3 = (Wall*) malloc(sizeof(Wall));
  *wall3 = create_wall_p(point_array[second_point + 1], point_array[0]);

  Sector* s = new_sector();
  sector_add_wall(s, wall1, 1, false);
  sector_add_wall(s, wall2, 1, false);
  sector_add_wall(s, *wall3, 1, false);
  sectors[num_sectors] = s; 

  if (array_size == second_point + 2) {
    node->sector = num_sectors;  
    ++num_sectors;
    return;
  }

  push(bla_stack, wall3);
  node->sector = -1;
  node->splitter = &bla_stack->walls[bla_stack->top];

  BSPNode* front = (BSPNode*) malloc(sizeof(BSPNode));
  front->splitter = NULL;
  front->sector = -1;
  front->front = NULL;
  front->back = NULL;
  node->front = front;

  BSPNode* back = (BSPNode*) malloc(sizeof(BSPNode));
  back->splitter = NULL;
  back->sector = -1;
  back->front = NULL;
  back->back = NULL;
  node->back = back;

  if (classify_point(*wall3, point_array[second_point + 1]) == 1) {
    front->sector = num_sectors;
    ++num_sectors;
    create_triangular_sectors(back, point_array, array_size, second_point + 1);
  } else {
    back->sector = num_sectors;
    ++num_sectors;
    create_triangular_sectors(front, point_array, array_size, second_point + 1);
  }
}

void make_triangles(BSPNode* node) {
  if (node == NULL) return;

  // Is not a sector.
  if (node->sector == -1) {
    make_triangles(node->front);
    make_triangles(node->back);
    return;
  }

  Sector* sector = sectors[node->sector];
  int num_points = sector_num_walls(sector);
  int arr_size = 0;
  Point point_array[100];

  WallList* temp = sector->walls;
  point_array[arr_size++] = temp->wall.p1;
  point_array[arr_size++] = temp->wall.p2;

  WallList* cur_wall = temp;
  Point cur_point = temp->wall.p2;
  while (arr_size < num_points) {
    temp = sector->walls;
    while (temp != NULL) {
      if (temp == cur_wall) {
        temp = temp->next;
        continue;
      }

      if (point_eq(cur_point, temp->wall.p1)) {
        point_array[arr_size++] = temp->wall.p2;
        cur_wall = temp;
        cur_point = temp->wall.p2;
      } else if (point_eq(cur_point, temp->wall.p2)) {
        point_array[arr_size++] = temp->wall.p1;
        cur_wall = temp;
        cur_point = temp->wall.p1;
      }
      temp = temp->next;
    }
  }

  create_triangular_sectors(node, point_array, arr_size, 1);

  // DEBUG.
  printf("New sector\n");
  for (int i = 0; i < num_points; ++i) {
    printf("%f %f, ", point_array[i].x, point_array[i].y);
  }
  printf("\n");
}

void create_sectors(WallStack* wall_stack) {
  bla_stack = wall_stack;

  Sector* world = new_sector();
  for (int i = 0; i < 4; i++)
    sector_add_wall(world, boundary_walls[i], 1, false);

  first_sector = 0;
  num_sectors = 0;
  create_sectors_aux(root, world);
  
  sectors[0]->color = 5;
  sectors[1]->color = 6;
  sectors[2]->color = 7;
  sectors[3]->color = 8;
  sectors[4]->color = 9;
  sectors[5]->color = 1;
  sectors[6]->color = 2;
  // sectors[4]->height = 200;
  // sectors[5]->height = 200;

  first_sector = num_sectors;
  make_triangles(root);
  sectors[20]->height = 200;
  sectors[21]->height = 200;

  // DEBUG.
  printf("num sectors: %d\n", num_sectors);
  for (int i = 0; i < num_sectors; ++i) { 
    printf("Sector %d\n", i);

    sectors[i]->heights[0] = 0;
    sectors[i]->heights[1] = 0;
    sectors[i]->heights[2] = 0;
    WallList* cur_wall = sectors[i]->walls;
    while (cur_wall != NULL) {
      Wall* w = &cur_wall->wall;
      printf("Wall %f %f %f %f\n", w->p1.x, w->p1.y, w->p2.x, w->p2.y);
      cur_wall = cur_wall->next;
    }
  }

  sectors[8]->heights[0] = 500;
  sectors[9]->heights[0] = 500;
  sectors[20]->heights[0] = 200;
  sectors[20]->heights[1] = 200;
  sectors[20]->heights[2] = 200;
  sectors[21]->heights[0] = 200;
  sectors[21]->heights[1] = 200;
  sectors[21]->heights[2] = 200;
}

#endif
