#ifndef VISUAL_ENGINE_H
#define VISUAL_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <map>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "textures.h"
using namespace glm;

#define EMPTY 0
#define WALL_FRONT_FACE 1
#define WALL_BACK_FACE 2
#define FLOOR 3
short canvas[1000][600];

void CleanCanvas() {
  for (int i = 0; i < 1000; ++i)
    for (int j = 0; j < 600; ++j)
      canvas[i][j] = EMPTY;
}

void ReserveCanvasWall(int x, int y1, int y2, short reservation) {
  if (y2 < y1) {
    int tmp = y2;
    y2 = y1;
    y1 = tmp;
  }

  if (x < 0 || x > 999) return;
  if (y1 < 0) y1 = 0;
  if (y2 > 599) y2 = 599;

  for (int i = y1; i < y2; i++) {
    if (canvas[x][i] == EMPTY)
      canvas[x][i] = reservation;
  }
}

void ReserveCanvasFloor(int y, int x1, int x2, short reservation) {
  if (x2 < x1) {
    int tmp = x2;
    x2 = x1;
    x1 = tmp;
  }

  if (y < 0 || y > 599) return;
  if (x1 < 0) x1 = 0;
  if (x2 > 999) x2 = 999;

  for (int i = x1; i < x2; i++) {
    if (canvas[i][y] == EMPTY || reservation == FLOOR)
      canvas[i][y] = reservation;
  }
}

double z_buffer[1000][600];
void ResetZBuffer() {
  for (int i = 0; i < 1000; ++i)
    for (int j = 0; j < 600; ++j)
      z_buffer[i][j] = 100000;
}

int windowWidth = 1000;
int windowHeight = 600;
unsigned char* data;
void DrawPixel(int x, int y, int r, int g, int b) {
  if (x >= windowWidth || x < 0 || y >= windowHeight || y < 0) return;
  y = windowHeight - 1 - y;

  data[y * windowWidth * 3 + x * 3]     = b;
  data[y * windowWidth * 3 + x * 3 + 1] = g;
  data[y * windowWidth * 3 + x * 3 + 2] = r;
}

void DrawPixelZ(int x, int y, int r, int g, int b, double distance) {
  if (x >= windowWidth || x < 0 || y >= windowHeight || y < 0) return;

  if (distance < z_buffer[x][y]) {
    z_buffer[x][y] = distance;
    DrawPixel(x, y, r, g, b);
  }
}

Texture textures[10];
void LoadTextures() {
  read_png_file("./textures/stone_wall.png", &textures[0]);  
  read_png_file("./textures/dirt.png", &textures[1]);  
}

void DrawTexture() {
  for (int y = 0; y < height; y++) {
    png_byte* row = textures[0].row_pointers[y];
    for (int x = 0; x < width; x++) {
      png_byte* ptr = &(row[x * 4]);
      DrawPixel(x, y, ptr[0], ptr[1], ptr[2]);
    }
  }
}

int rgb[20][3];
void DrawWall(int x, int y1, int y2, int color, double texture_x, double wall_height, double proj_height) {
  if (y2 < y1) {
    int tmp = y2;
    y2 = y1;
    y1 = tmp;
  }

  int before_y = y1;

  if (x < 0 || x > 999) return;
  if (y1 < 0) y1 = 0;
  if (y2 > 599) y2 = 599;

  int tex_x = (int) (5 * texture_x) % textures[0].width;
  double tex_y = 0;

  int actual_height = (int) wall_height % textures[0].height;
  double y_step = (actual_height / 1.6f) / proj_height;

  for (int i = before_y; i < y1; ++i) {
    tex_y += y_step;
  }

  for (int i = y1; i < y2; ++i) {
    if (canvas[x][i] == EMPTY) {
      int itex_y = (int) tex_y % actual_height;
      png_byte* row = textures[0].row_pointers[itex_y];
      png_byte* ptr = &(row[tex_x * 4]);
      DrawPixel(x, i, ptr[0], ptr[1], ptr[2]);
      // DrawPixel(x, i, rgb[color][0], rgb[color][1], rgb[color][2]);
    }
    tex_y += y_step;
  }
}

// Bresenham's.
void DrawLine(int x1, int y1, int x2, int y2, int color) {
  int x, y, xe, ye, i;
  int dx = x2 - x1;
  int dy = y2 - y1;
  int dx1 = fabs(dx);
  int dy1 = fabs(dy);
  int px = 2 * dy1 - dx1;
  int py = 2 * dx1 - dy1;
  if (dy1 <= dx1) {
    if (dx >= 0) {
      x = x1; y = y1; xe = x2;
    } else {
      x = x2; y = y2; xe = x1;
    }
    DrawPixel(x, y, rgb[color][0], rgb[color][1], rgb[color][2]);

    for (i = 0; x < xe; i++) {
      x = x + 1;
      if (px < 0) {
        px = px + 2 * dy1;
      } else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
          y = y + 1;
        } else {
          y = y - 1;
        }
        px = px + 2 * (dy1 - dx1);
      }
      DrawPixel(x, y, rgb[color][0], rgb[color][1], rgb[color][2]);
    }
  } else {
    if (dy >= 0) {
      x = x1;
      y = y1;
      ye = y2;
    } else {
      x = x2;
      y = y2;
      ye = y1;
    }
    DrawPixel(x, y, rgb[color][0], rgb[color][1], rgb[color][2]);

    for (i = 0; y < ye; i++) {
      y = y + 1;
      if (py <= 0) {
        py = py + 2 * dx1;
      } else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
          x = x + 1;
        } else {
          x = x - 1;
        }
        py = py + 2 * (dx1 - dy1);
      }
      DrawPixel(x, y, rgb[color][0], rgb[color][1], rgb[color][2]);
    }
  }
}

void FillRect(int x1, int y1, int x2, int y2, int color) {
  if (x2 < x1) {
    int tmp = x2; x2 = x1; x1 = tmp;
  }

  if (y2 < y1) {
    int tmp = y2; y2 = y1; y1 = tmp;
  }

  for (int x = x1; x < x2; ++x) {
    for (int y = y1; y < y2; ++y) {
      DrawPixel(x, y, rgb[color][0], rgb[color][1], rgb[color][2]);
    }
  }
}

void DrawCircle(int x0, int y0, int radius, int color) {
  int x = radius - 1;
  int y = 0;
  int dx = 1;
  int dy = 1;
  int err = dx - (radius << 1);

  while (x >= y) {
    DrawPixel(x0 + x, y0 + y, rgb[color][0], rgb[color][1], rgb[color][2]);
    DrawPixel(x0 + y, y0 + x, rgb[color][0], rgb[color][1], rgb[color][2]);
    DrawPixel(x0 - y, y0 + x, rgb[color][0], rgb[color][1], rgb[color][2]);
    DrawPixel(x0 - x, y0 + y, rgb[color][0], rgb[color][1], rgb[color][2]);
    DrawPixel(x0 - x, y0 - y, rgb[color][0], rgb[color][1], rgb[color][2]);
    DrawPixel(x0 - y, y0 - x, rgb[color][0], rgb[color][1], rgb[color][2]);
    DrawPixel(x0 + y, y0 - x, rgb[color][0], rgb[color][1], rgb[color][2]);
    DrawPixel(x0 + x, y0 - y, rgb[color][0], rgb[color][1], rgb[color][2]);

    if (err <= 0) {
      y++;
      err += dy;
      dy +=2;
    }
    if (err > 0) {
      x--;
      dx += 2;
      err += (-radius << 1) + dx;
    }
  }
}

void DrawFloor(int y, int x1, int x2, Point floor_p1, Point floor_p2, int color) {
  if (x2 < x1) {
    int tmp = x2;
    x2 = x1;
    x1 = tmp;
  }
  int before_x1 = x1;
  int before_x2 = x2;

  if (color > 20 || color < 0) color = 0;

  if (y < 0 || y > 599) return;
  if (x1 < 0) x1 = 0;
  if (x2 > 999) x2 = 999;

  double step_size_x = (floor_p2.x - floor_p1.x) / (before_x2 - before_x1);
  
  double step_size_y;
  if (step_size_x != 0) {
    step_size_y = ((floor_p2.y - floor_p1.y) / (floor_p2.x - floor_p1.x)) * step_size_x;
  } else {
    step_size_y = (floor_p2.y - floor_p1.y) / (before_x2 - before_x1);
  }

  double tex_x = floor_p1.x;
  double tex_y = floor_p1.y;
  for (int i = before_x1; i < x; ++i) {
    tex_x += step_size_x;
    tex_y += step_size_y;
  }

  for (int i = x1; i < x2; ++i) {
    if (canvas[i][y] == EMPTY || canvas[i][y] == WALL_BACK_FACE) {
      int actual_tex_x = (int) tex_x % textures[1].width;
      int actual_tex_y = (int) tex_y % textures[1].height;

      png_byte* row = textures[1].row_pointers[actual_tex_y];
      png_byte* ptr = &(row[actual_tex_x * 4]);
      DrawPixel(i, y, ptr[0], ptr[1], ptr[2]);
      // DrawPixel(i, y, rgb[color][0], rgb[color][1], rgb[color][2]);
    }
    tex_x += step_size_x;
    tex_y += step_size_y;
  }
}

void ClearProjectionScreen() {
  for (int i = 0; i < windowHeight; ++i) {
    for (int j = 0; j < windowWidth; ++j) {
      data[i * windowWidth * 3 + j * 3]     = 0;
      data[i * windowWidth * 3 + j * 3 + 1] = 0;
      data[i * windowWidth * 3 + j * 3 + 2] = 0;
    }
  }
}

void InitVisualEngine() {
  rgb[0][0]  = 255; rgb[0][1]  = 0; rgb[0][2]  = 255;
  rgb[1][0]  = 255; rgb[1][1]  = 255; rgb[1][2]  = 0;
  rgb[2][0]  = 0; rgb[2][1]  = 255; rgb[2][2]  = 255;
  rgb[3][0]  = 255; rgb[3][1]  = 0; rgb[3][2]  = 0;
  rgb[4][0]  = 0; rgb[4][1]  = 0; rgb[4][2]  = 255;
  rgb[5][0]  = 0; rgb[5][1]  = 255; rgb[5][2]  = 0;
  rgb[6][0]  = 255; rgb[6][1]  = 100; rgb[6][2]  = 100;
  rgb[7][0]  = 255; rgb[7][1]  = 0; rgb[7][2]  = 100;
  rgb[8][0]  = 255; rgb[8][1]  = 255; rgb[8][2]  = 255;
  rgb[9][0]  = 200; rgb[9][1]  = 125; rgb[9][2]  = 23;
  rgb[10][0] = 211; rgb[10][1] = 23; rgb[10][2] = 55;
  rgb[11][0] = 19; rgb[11][1] = 92; rgb[11][2] = 123;
  rgb[12][0] = 2; rgb[12][1] = 244; rgb[12][2] = 100;

  LoadTextures();
}

#endif
