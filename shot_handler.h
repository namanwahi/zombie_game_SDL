#ifndef SHOT_HANDLER_H
#define SHOT_HANDLER_H

#include "line.h"
#include "Enemy.h"
#include "player.h"
#include "geometry_utils.h"

void handleShot(const Player& shooter, Enemy& enemy) {
  if (!shooter.isShotsEmpty()) {
    const Line& last_shot = shooter.lastShot();
    
    //if the line was just shot
    if (last_shot.life_on_screen == start_shot_life) {
      
      //if the shot hit the Enemy kill the Enemy
      if (enemy.intersectsLine(last_shot) && findQuadrant(shooter.getCentre(), enemy.getCentre())
          == findQuadrant(last_shot.angle_d)) {
        enemy.reduceHealth();
      }
    }
    
  }
}



#endif

