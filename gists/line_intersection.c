#include <math.h>
#include <stdio.h>

typedef struct {
  double x;
  double y;
} Point;

typedef struct {
  Point p1;
  Point p2;
} Line;

Point line_intersection(Line l1, Line l2) {
  double a1 = l1.p2.y - l1.p1.y;
  double a2 = l2.p2.y - l2.p1.y;
  double b1 = l1.p2.x - l1.p1.x;
  double b2 = l2.p2.x - l2.p1.x;
  double c1 = a1 * l1.p1.x + b1 * l1.p1.y;
  double c2 = a2 * l2.p1.x + b2 * l2.p1.y;

  Point p; 
  double det = a1 * b2 - a2 * b1;
  if (det == 0) {
    return p;
  } else {
    p.x = (b2 * c1 - b1 * c2) / det; 
    p.y = (a1 * c2 - a2 * c1) / det; 
  }

  return p;
}

int main () {
  Line l1;
  l1.p1.x = 10;
  l1.p1.y = 10;
  l1.p2.x = 100;
  l1.p2.y = 10;

  Line l2;
  l2.p1.x = 50;
  l2.p1.y = 50;
  l2.p2.x = 50;
  l2.p2.y = -50;

  Point p = line_intersection(l1, l2);
  printf("x: %f, y: %f\n", p.x, p.y);
  return 0;
}
