#ifndef LINE_H
#define LINE_H

#include "SDL.h"

//how many frames the shot stays on screen after it has been shot
static const int start_shot_life = 100;

typedef struct ShootLine {
  SDL_Point p1;
  SDL_Point p2;
  int life_on_screen;
  int angle_d;
  ShootLine(int x1, int y1, int x2, int y2, int angle) : p1{x1, y1}, p2{x2, y2},  life_on_screen(start_shot_life), angle_d(angle) {}
} Line;

#endif
