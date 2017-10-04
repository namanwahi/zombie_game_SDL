#ifndef TEXTURE_WRAPPER_H
#define TEXTURE_WRAPPER_H

#include <string>
#include "SDL.h"
#include "SDL_ttf.h"
#include <memory>

class TextureWrapper {
  public:
  TextureWrapper();

  bool loadFromFile(std::string path, SDL_Renderer* renderer, int width);

  bool loadTTFTexture(std::string text, TTF_Font * font, SDL_Color color, SDL_Renderer* renderer);

  //Renders texture at given point
  void render(int x, int y, SDL_Renderer* renderer, SDL_Rect* clip = nullptr, double angle = 0.0, SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

  //Gets image dimensions
  int getWidth();
  int getHeight();

  private:

  std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> pTexture;
  
  //Image dimensions
  int width;
  int height;
};



#endif
