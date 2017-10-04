#ifndef RESOURCE_BAR_H
#define RESOURCE_BAR_H

#include "player.h"
#include "SDL.h"

//forward declaration of Player.h
class Player;

class resourceBar {
 public:

	static constexpr double PROPORTIONAL_WIDTH = 0.5;
	static constexpr double PROPORTIONAL_BAR_HEIGHT = 0.05;
	
	resourceBar(int width, int height) : screen_width(width), screen_height(height) {}

	//renders the bar based on the stats of the renderer
	void render(const Player& dot, SDL_Renderer* renderer);
	
 private:
	int screen_width;
	int screen_height;
	
};

#endif
