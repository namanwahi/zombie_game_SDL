#ifndef GEOMETRY_UTILS
#define GEOMETRY_UTILS

#include "SDL.h"
#include <cmath>



enum Quadrant {
 FIRST_QUADRANT = 1,
 SECOND_QUADRANT,
 THIRD_QUADRANT,
 FOURTH_QUADRANT 
};

double distanceFromPointToLine(SDL_Point p, double A, double B, double C);

//calculate the equation Ax + By + C = 0 between p1 and p2
void calculateEquation(SDL_Point p1, SDL_Point p2, double* A, double* B, double* C);

Quadrant findQuadrant(SDL_Point origin, SDL_Point target);

Quadrant findQuadrant(double angle_d);

double calculateAngleDegrees(SDL_Point p1, SDL_Point p2);

double findDistance(SDL_Point p1, SDL_Point p2);

#endif
