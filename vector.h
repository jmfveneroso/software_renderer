#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

#define false 0
#define true 1
#define bool short

typedef struct {
  double x;
  double y;
  bool null;
} Point;

typedef struct {
  Point p1;
  Point p2;
  Point normal;
  unsigned long int color;
  double height;
} Wall;

Point create_point(double x, double y) {
  Point p;
  p.x = x;
  p.y = y;
  p.null = false;
  return p;
}

Wall create_wall(double x1, double y1, double x2, double y2) {
  Wall w;
  w.p1 = create_point(x1, y1);
  w.p2 = create_point(x2, y2);
  return w;
}

// It is possible to solve it with a cross product between the vector and 
// a virtual (0, 0, 1) vector.
Point get_normal(Wall wall) {
  Point vector;
  vector.x = -(wall.p1.y - wall.p2.y);
  vector.y = wall.p1.x - wall.p2.x;
  double distance = sqrt(vector.x * vector.x + vector.y * vector.y);
  vector.x = 20 * vector.x / distance;
  vector.y = 20 * vector.y / distance;
  return vector;
}

Point vec_add(Point p1, Point p2) {
  Point result;
  result.x = p1.x + p2.x;
  result.y = p1.y + p2.y;
  return result;
}

Point vec_sub(Point p1, Point p2) {
  Point result;
  result.x = p1.x - p2.x;
  result.y = p1.y - p2.y;
  return result;
}

Point vec_scalar(double scalar, Point p1) {
  Point result;
  result.x = scalar * p1.x;
  result.y = scalar * p1.y;
  return result;
}

double dot_product(Point p1, Point p2) {
  return p1.x * p2.x + p1.y * p2.y;
}

Point get_normal_vec(Point vec) {
  Point vector;
  vector.x = -vec.y;
  vector.y = vec.x;
  return vector;
}

double vec_norm(Point vec) {
  return sqrt(vec.x * vec.x + vec.y * vec.y);
}

Point intersection(Point p1, Point p2, Point p3, Point p4) {
  Point target = vec_sub(p2, p1);
  Point normal = get_normal_vec(vec_sub(p4, p3));

  double numerator = dot_product(vec_sub(p3, p1), normal);
  double denominator = dot_product(target, normal);
  if (denominator == 0) {
    Point p;
    p.null = true;
    return p;
  }

  double s = (double) numerator / denominator;
  Point p = vec_add(p1, vec_scalar(s, target));
  p.null = false;
  return p;
}

Point line_intersection(Wall l1, Wall l2, bool* parallel) {
  double a1 = l1.p2.y - l1.p1.y;
  double a2 = l2.p2.y - l2.p1.y;
  double b1 = l1.p2.x - l1.p1.x;
  double b2 = l2.p2.x - l2.p1.x;
  double c1 = a1 * l1.p1.x + b1 * l1.p1.y;
  double c2 = a2 * l2.p1.x + b2 * l2.p1.y;

  Point p; 
  double det = a1 * b2 - a2 * b1;
  if (det == 0) {
    *parallel = true;
    return p;
  } else {
    p.x = (b2 * c1 - b1 * c2) / det; 
    p.y = (a1 * c2 - a2 * c1) / det; 
    if (fabs(p.x) < 0.0000001) p.x = 0;
    if (fabs(p.y) < 0.0000001) p.y = 0;
  }

  *parallel = false;
  return p;
}

Point segment_intersection(Wall l1, Wall l2) {
  Point p = intersection(l1.p1, l1.p2, l2.p1, l2.p2); 
  if (p.null) {
    return p;
  }

  double l1_norm = fabs(vec_norm(vec_sub(l1.p1, l1.p2)));  
  double l2_norm = fabs(vec_norm(vec_sub(l2.p1, l2.p2)));  

  if (fabs(vec_norm(vec_sub(p, l1.p1))) > l1_norm || fabs(vec_norm(vec_sub(p, l1.p2))) > l1_norm)
    p.null = true;
  else if (fabs(vec_norm(vec_sub(p, l2.p1))) > l2_norm || fabs(vec_norm(vec_sub(p, l2.p2))) > l2_norm)
    p.null = true;
  else
    p.null = false;

  return p;
}


#endif
