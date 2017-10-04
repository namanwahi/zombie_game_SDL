#ifndef PLAYER_H
#define PLAYER_H

#include "SDL.h"
#include "texture_wrapper.h"
#include <list>
#include "line.h"
#include <cmath>
#include "resource_bar.h"

class Player {
	friend class resourceBar;

public:
	static constexpr int WIDTH = 100;
	static constexpr int HEIGHT = WIDTH;

	//how much the player slows down while aiming
	static constexpr double AIM_SLOW_FACTOR = 0.6;

	static constexpr double MAX_ACCURACY_ANGLE = 15;
	static constexpr double MIN_ACCURACY_ANGLE = 2;
	static constexpr double AIM_GUIDE_LENGTH = 250;
	static constexpr int START_LIFE_POINTS = 1000;
	
	//Maximum axis velocity of the player
	static constexpr double PLAYER_VEL = 2.0;

	//Initializes the variables
	Player(int x_pos, int y_pos);

	void handleEvent(SDL_Event &e);

	void move(const int, const int);

	SDL_Point getCentre() const {
		SDL_Point ret = {pos_x + WIDTH/2, pos_y + HEIGHT/2};
		return ret;
	}

	SDL_Point getVisibleCentre() const {
		SDL_Point actual_centre = getCentre();
		
		double distance_from_centre = WIDTH * 0.22;
		SDL_Point res = {actual_centre.x - distance_from_centre * cos(angle_d * 3.14159265359 / 180.0),
										 actual_centre.y - distance_from_centre * sin(angle_d * 3.14159265359 / 180.0)};
		return res;
	}

	void render(SDL_Renderer* renderer, TextureWrapper& texture);

	//controls aim accuracy (how large the angle is)
	void adjustAimAccuracy();

	const Line& lastShot() const {
		return shots.back();
	}

	//are there any shots remaining on screen
	bool isShotsEmpty() const{
		return shots.empty();
	}

	//enum to store the moving state
	enum MovingState {
		MOVING_FORWARD,
		STILL,
		MOVING_BACKWARD
	};

	MovingState getMovingState() const {
		return mov_state;
	}


	void takeDamage(int dmg) {
		life_points -= dmg;
		if (life_points < 0) {
			life_points = 0;
		}
	}

	bool isDead() const {
		return life_points <= 0;
	}
	
private:

	MovingState mov_state;

	int life_points;
	
	//vector to store all the bullet shorts
	std::list<Line> shots;

	//resets the accuracy of the aim
	void resetAccAngle() {
		acc_angle_d = MAX_ACCURACY_ANGLE;
	}
	
	//lowers the accuracy angle of the aim
	void reduceAccAngle() {
		const double reduce_speed = 0.1;
		acc_angle_d -= reduce_speed;
		if (acc_angle_d < MIN_ACCURACY_ANGLE) {
			acc_angle_d = MIN_ACCURACY_ANGLE;
		}
	}

	
	//adds a line to the vector
	void shoot();
	
	//boolean to determine if the player is aiming
	bool is_aiming;

	//The X and Y offsets of the player
	double pos_x, pos_y;

	double forward_vel;

	//the clockwise angle
	double angle_velocity_d;
	double angle_d;

	//the accuracy angle
	double acc_angle_d;
};


#endif
