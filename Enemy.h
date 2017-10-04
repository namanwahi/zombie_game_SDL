#ifndef ENEMY_H
#define ENEMY_H

#include "player.h"
#include "texture_wrapper.h"
#include <SDL2/SDL.h>
#include "line.h"
#include <list>
#include <memory>

class Enemy {

public:
  static constexpr int ENEMY_WIDTH = 40;
  static constexpr int ENEMY_HEIGHT = 40;
	static constexpr double SPEED = 0.25;


  Enemy(double x, double y, int lp = 1, bool is = false) : x_pos(x), y_pos(y), has_been_hit(false), is_dead(false), angle_d(0), life_points(lp), is_super(is) { }

  virtual ~Enemy() {};

	//move towards the player without colliding with other enemies
  virtual void move(const Player &target, const std::list<std::unique_ptr<Enemy>> &enemies);

  void render(SDL_Renderer* renderer, TextureWrapper& texture) const;

  bool intersectsLine(const Line &line) const;
  
  SDL_Point getCentre() const {
    SDL_Point ret = {x_pos + ENEMY_WIDTH/2, y_pos + ENEMY_HEIGHT/2};
    return ret;
  }

	double getAngleDegrees() const {
		return angle_d;
	}
	
  void reduceHealth() {
		life_points--;
		if (life_points <= 0) {
		  is_dead = true;	
		}
  }

  bool isDead() const {
    return is_dead;
  }

	//TODO:refactor to enable polymorphic behaviour
	const bool is_super;

protected:

  virtual void moveHelper(const Player &target, const std::list<std::unique_ptr<Enemy>> &enemies, double speed_factor);

private:


	int life_points;
	
	bool willCollideWithEnemies(const std::list<std::unique_ptr<Enemy>> &enemies);
	
	//angle in degrees
	double angle_d;
	
  double x_pos;
  double y_pos;

	bool has_been_hit;

  //boolean to determine if it is dead
  bool is_dead;


};

class SuperEnemy : public Enemy {
public:

    SuperEnemy(double x, double y) : Enemy(x, y, 2, true)  {}
    virtual ~SuperEnemy() {}

    virtual void move(const Player &target, const std::list<std::unique_ptr<Enemy>> &enemies) override;

private:
  //set super Enemy with double speed
  static constexpr int SUPER_SPEED  = 5;
};



#endif
