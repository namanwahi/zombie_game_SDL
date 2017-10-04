#include "Enemy.h"
#include "player.h"
#include "line.h"
#include "SDL.h"
#include "geometry_utils.h"
#include <memory>
#include <iostream>


void Enemy::moveHelper(const Player &target, const std::list<std::unique_ptr<Enemy>> &enemies, double speed_factor) {
  static const double pi = 3.14159265;
  this->angle_d = calculateAngleDegrees(getCentre(), target.getCentre());
  if (!willCollideWithEnemies(enemies)) {
    x_pos += speed_factor * SPEED * cos(angle_d * pi/180.0);
    y_pos += speed_factor * SPEED * sin(angle_d * pi/180.0);
  }
}

void Enemy::move(const Player &target, const std::list<std::unique_ptr<Enemy>> &enemies) {
  moveHelper(target, enemies, 1.0);
}

void Enemy::render(SDL_Renderer* renderer, TextureWrapper& texture) const {
  texture.render(x_pos, y_pos, renderer, nullptr, angle_d);
}

bool Enemy::intersectsLine(const Line &line) const {

  //Ax + By + C = 0
  double A, B, C;
  calculateEquation(line.p1, line.p2, &A, &B, &C);

  double d = distanceFromPointToLine(getCentre(), A, B, C);
  
  return d < ENEMY_WIDTH;
}

bool Enemy::willCollideWithEnemies(const std::list<std::unique_ptr<Enemy>> &enemies) {
  
  auto it = enemies.begin();
  while (it != enemies.end()) {
    if (this != (*it).get()) {
      //if too close to one of the other enemies and is facing it return true
      if (findDistance(this->getCentre(), (*it)->getCentre()) <= ENEMY_WIDTH
          && findQuadrant(this->getCentre(), (*it)->getCentre()) == findQuadrant(this->getAngleDegrees())) {
        return true;
      }
    }
    it++;
  }
  return false;
}



void SuperEnemy::move(const Player &target, const std::list<std::unique_ptr<Enemy>> &enemies) {
  Enemy::moveHelper(target, enemies, SuperEnemy::SUPER_SPEED);
}
