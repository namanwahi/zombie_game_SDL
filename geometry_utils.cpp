#include "geometry_utils.h"
#include "SDL.h"
#include <cmath>

static constexpr int PI = 3.14159265;

double distanceFromPointToLine(SDL_Point p, double A, double B, double C) {
  double m = p.x;
  double n = p.y;

  double numerator = abs(A * m + B * n + C);
  double denominator = sqrt(pow(A, 2) + pow(B, 2));

  return numerator/denominator;
}

void calculateEquation(SDL_Point p1, SDL_Point p2, double* A, double* B, double* C) {
  double x1 = p1.x;
  double x2 = p2.x;
  
  double y1 = p1.y;
  double y2 = p2.y;

  *A = y1 - y2;
  *B = x2 - x1;
  *C = (x1 * y2) - (x2 * y1);
}

Quadrant findQuadrant(SDL_Point origin, SDL_Point target) {
  
  int y_diff = target.y - origin.y;
  int x_diff = target.x - origin.x;

  bool y_positive = y_diff >= 0;
  bool x_positive = x_diff >= 0;


  if (x_positive && y_positive) {
    return FIRST_QUADRANT;
  } else if (!x_positive && y_positive) {
    return SECOND_QUADRANT;
  } else if (!x_positive && !y_positive) {
    return THIRD_QUADRANT;
  } else {
    return FOURTH_QUADRANT;
  }
}

Quadrant findQuadrant(double angle_d) {
  if (angle_d >= 0 && angle_d < 90.0) {
    return FIRST_QUADRANT;
  } else if (angle_d >= 90.0 && angle_d < 180.0) {
    return SECOND_QUADRANT;
  } else if (angle_d >= 180.0 && angle_d < 270.0) {
    return THIRD_QUADRANT;
  } else {
    return FOURTH_QUADRANT;
  }
}

double calculateAngleDegrees(SDL_Point origin, SDL_Point target) {
  double y_diff = target.y - origin.y;
  double x_diff = target.x - origin.x;
  double ret =  atan2(y_diff, x_diff) * 180.0/PI;
  if (ret < 0) {
    ret = 360.0 + ret;
  }
  return ret;
}

double findDistance(SDL_Point p1, SDL_Point p2) {
  return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}
