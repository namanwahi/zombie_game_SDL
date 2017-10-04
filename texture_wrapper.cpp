#include "texture_wrapper.h"
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <iostream>
#include <memory>

TextureWrapper::TextureWrapper() : width(0), height(0), pTexture(nullptr, &SDL_DestroyTexture) {}

bool TextureWrapper::loadFromFile(std::string path, SDL_Renderer* renderer, int width) {

	//Load image at specified path
	std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> loadedSurface(IMG_Load(path.c_str()), &SDL_FreeSurface);
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> newTexture(SDL_CreateTextureFromSurface(renderer, loadedSurface.get()), &SDL_DestroyTexture);

	if (!loadedSurface) {
		std::cerr << IMG_GetError();
  } else {
    //Color key image, remove background white colour
    SDL_SetColorKey(loadedSurface.get(), SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0xFF, 0xFF));
    if (!newTexture) {
			std::cerr << SDL_GetError();
    } else if (width == 0){
      //Get image dimensions
      this->width = loadedSurface->w;
      this->height = loadedSurface->h;
    } else {
			this->width = width;
			this->height = width * ((float) loadedSurface->h/ (float) loadedSurface->w);
    }
  }
  //Return success
  pTexture = std::move(newTexture);
  return pTexture != nullptr;
}

bool TextureWrapper::loadTTFTexture(std::string text, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer) {
	std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> loadedSurface(TTF_RenderText_Solid(font, text.c_str(), color) ,SDL_FreeSurface);
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> newTexture(SDL_CreateTextureFromSurface(renderer, loadedSurface.get()), SDL_DestroyTexture);
	if (!loadedSurface) {
		std::cerr << TTF_GetError();
	} else {
		if (!newTexture) {
			std::cerr << SDL_GetError();
		} else {
			this->width = loadedSurface->w;
			this->height = loadedSurface->h;
		}
	}

	//Return success
	pTexture = std::move(newTexture);
	return pTexture != nullptr;
}

void TextureWrapper::render(int x, int y, SDL_Renderer* renderer, SDL_Rect * clip, double angle_velocity_d, SDL_Point * center, SDL_RendererFlip flip) {
  //Set rendering space and render to screen
  SDL_Rect renderQuad = {
    x,
    y,
    width,
		height
  };

  //Set clip rendering dimensions
  if (clip) {
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }

  //Render to screen
  SDL_RenderCopyEx(renderer, pTexture.get(), clip, &renderQuad, angle_velocity_d, center, flip);
}

int TextureWrapper::getWidth() {
  return width;
}

int TextureWrapper::getHeight() {
  return height;
}
