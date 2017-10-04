#include "resource_bar.h"
#include "SDL.h"

void resourceBar::render(const Player& dot, SDL_Renderer* renderer) {
  int y_buffer = screen_height * 0.01;
  int x_buffer = screen_width * 0.01;
  //save backup of the colour in the renderer
  Uint8 backup_r, backup_g, backup_b, backup_a;
  SDL_GetRenderDrawColor(renderer, &backup_r, &backup_g, &backup_b, &backup_a);

  //draw the health bar as green rect
  SDL_Rect health_bar;
  health_bar.x = x_buffer;
  health_bar.y = y_buffer;
  health_bar.h = screen_height * PROPORTIONAL_BAR_HEIGHT;
  health_bar.w = ((double) dot.life_points / (double) dot.START_LIFE_POINTS) * screen_width * PROPORTIONAL_WIDTH;

  SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, 0xff);
  SDL_RenderFillRect(renderer, &health_bar);

  SDL_Rect health_outline;
  health_bar.x = x_buffer;
  health_bar.y = y_buffer;
  health_bar.h = screen_height * PROPORTIONAL_BAR_HEIGHT;
  health_bar.w = screen_width * PROPORTIONAL_WIDTH;

  SDL_SetRenderDrawColor(renderer, 0x22, 0x8b, 0x22, 0xff);
  SDL_RenderDrawRect(renderer, &health_bar);

  //draw health bar
  SDL_SetRenderDrawColor(renderer, backup_r, backup_g, backup_b, backup_a);
  
}
  
  
