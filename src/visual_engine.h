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
using namespace glm;

int rgb[20][3];

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
  DrawPixel(x, y, r, g, b);
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
}

#endif
