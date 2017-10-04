#include "player.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "rand_utils.h"

static const double PI = 3.14159;


Player::Player(int x_pos, int y_pos) : pos_x(x_pos), pos_y(y_pos), forward_vel(0.0) , is_aiming(false),
angle_velocity_d(0.0), angle_d(0.0), acc_angle_d(MAX_ACCURACY_ANGLE), mov_state(STILL), life_points(START_LIFE_POINTS) {}

void Player::handleEvent(SDL_Event & e) {
  const double rot_vel = 0.75;
  //If a key was pressed
  if( e.type == SDL_KEYDOWN && e.key.repeat == 0 ) {
    //Adjust the velocity
    switch(e.key.keysym.sym) {
      case SDLK_w:
        forward_vel -= PLAYER_VEL;
        mov_state = MOVING_FORWARD;
        break;
      case SDLK_s:
        forward_vel += PLAYER_VEL;
        mov_state = MOVING_BACKWARD;
        break;
      case SDLK_a:
        angle_velocity_d -= rot_vel;
        break;
      case SDLK_d:
        angle_velocity_d += rot_vel;
        break;
      case SDLK_SPACE:
        is_aiming = true;
        break;
    }
    //If a key was release
  } else if( e.type == SDL_KEYUP && e.key.repeat == 0 ) {
    //Adjust the velocity
    switch( e.key.keysym.sym ) {
      case SDLK_w:
        forward_vel += PLAYER_VEL;
        mov_state = STILL;
        break;
      case SDLK_s:
        forward_vel -= PLAYER_VEL;
        mov_state = STILL;
        break;
      case SDLK_a:
        angle_velocity_d += rot_vel;
        break;
      case SDLK_d:
        angle_velocity_d -= rot_vel;
        break;
      case SDLK_SPACE:
        is_aiming = false;
        shoot();
        break;
    }
  }

}


void Player::move(const int SCREEN_WIDTH, const int SCREEN_HEIGHT) {
  
  double vel;
  double rot_vel;

  //if character is aiming then slow down
  if (is_aiming) {
    vel = forward_vel * AIM_SLOW_FACTOR;
    rot_vel = angle_velocity_d * AIM_SLOW_FACTOR;
  } else {
    vel = forward_vel;
    rot_vel = angle_velocity_d;
  }
    
  //Move the player left or right
  pos_x -= vel * cos(PI * angle_d / 180.0);
  
  //If the player went too far to the left or right
  if (pos_x < 0) {
    pos_x = 0;
  } else if (pos_x + WIDTH > SCREEN_WIDTH) {
    //Move back
    pos_x = SCREEN_WIDTH - WIDTH;
  }

  //Move the player up or down
  pos_y -= vel * sin(PI *  angle_d / 180.0);

  //If the player went too far up or down
  if (pos_y < 0) {
    pos_y = 0;
  } else if (pos_y + HEIGHT > SCREEN_HEIGHT) {
    pos_y = SCREEN_HEIGHT - HEIGHT;
  }

  angle_d += rot_vel;
  if (angle_d > 360.0) {
    angle_d -= 360.0;
  } else if (angle_d < 0) {
    angle_d = 360 - angle_d;
  }

}


void Player::render(SDL_Renderer* renderer, TextureWrapper& texture) {


  //calculate both angles
  double upper_angle = (acc_angle_d - angle_d) * PI / 180.0;
  double lower_angle = (-angle_d - acc_angle_d) * PI / 180.0;

  int x =  pos_x + WIDTH/2;
  int y =  pos_y + HEIGHT/2;
  
  SDL_SetRenderDrawColor(renderer, 0, 0 , 255, 255);
  SDL_RenderDrawLine(renderer, x, y, x + AIM_GUIDE_LENGTH * cos(upper_angle)
                     , y - AIM_GUIDE_LENGTH * sin(upper_angle));
    SDL_RenderDrawLine(renderer, x, y, x + AIM_GUIDE_LENGTH * cos(lower_angle)
                     , y - AIM_GUIDE_LENGTH * sin(lower_angle));


  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        std::list<Line>::iterator it = shots.begin();
        while (it != shots.end()) {
          SDL_RenderDrawLine(renderer, it->p1.x, it->p1.y, it->p2.x, it->p2.y);

          (it->life_on_screen)--;
          if (it->life_on_screen == 0) {
            it = shots.erase(it);
          } else {
            it++;
          }
        }

  
  //Show the texture
  texture.render(pos_x, pos_y, renderer, NULL, angle_d);

  SDL_RenderDrawPoint(renderer, getVisibleCentre().x, getVisibleCentre().y);
  
  //restore colour
  SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255);
  
}

void Player::adjustAimAccuracy() {

    if (is_aiming) {
      reduceAccAngle();
    } else {
      resetAccAngle();
    }
}

void Player::shoot() {

  const double shot_length = 800;
  
  double x1 = pos_x + WIDTH/2;
  double y1 = pos_y + HEIGHT/2;

  int angle_d_int = angle_d - (int) acc_angle_d  + random(2 * acc_angle_d);

  
  double x2 = x1 + shot_length * cos (angle_d_int * PI /180.0);
  double y2 = y1 + shot_length * sin (angle_d_int * PI /180.0);
  Line l(x1, y1, x2, y2, angle_d_int);

  shots.push_back(l);
}
