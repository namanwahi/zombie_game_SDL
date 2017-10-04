#include "SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include <iostream>
#include <string>
#include <cmath>
#include "texture_wrapper.h"
#include "player.h"
#include "Enemy.h"
#include "shot_handler.h"
#include <list>
#include "rand_utils.h"
#include "geometry_utils.h"
#include "resource_bar.h"
#include <memory>
#include <array>

constexpr int SCREEN_WIDTH = 960;
constexpr int SCREEN_HEIGHT = 640;

bool initialiseGame();
bool loadMedia();
void close();
void randomSpawnEnemy();
void enemyAttackHandler();
void renderScore(int x_pos, int y_pos);
void showEndgameScreen();
void resetGame();

static constexpr int PLAYER_FRAMES_COUNT = 4;
static constexpr int FRAME_CHANGE_ITERATIONS = 80;
static constexpr int ENEMY_FRAMES_COUNT = 4;
static constexpr int SUPER_ENEMY_FRAMES = 4;


using window_ptr = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
//window to render to
static window_ptr g_window(nullptr, &SDL_DestroyWindow);

using renderer_ptr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
//renderer
static renderer_ptr g_renderer(nullptr, &SDL_DestroyRenderer);

using font_ptr = std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>;
//font this game uses
static font_ptr g_font(nullptr, &TTF_CloseFont);

//all the textures used in this game
static std::array<TextureWrapper, PLAYER_FRAMES_COUNT> g_player_texture;
static std::array<TextureWrapper, ENEMY_FRAMES_COUNT> g_enemy_texture;
static std::array<TextureWrapper, SUPER_ENEMY_FRAMES> g_super_enemy_texture;
static TextureWrapper g_background_texture;


static std::list<std::unique_ptr<Enemy>> enemies;
static Player g_player{SCREEN_WIDTH, SCREEN_HEIGHT/2};

//the players current score
static int g_score = 0;


int main(int argc, char * args[]) {

  if (!initialiseGame()) {
    std::cerr << "Initialiser Fail\n";
  } else {

    if (!loadMedia()) {
      std::cerr << "Failed to load media";
    } else {
      bool quit = false;
      SDL_Event sdl_event;

      resourceBar r_bar(SCREEN_WIDTH, SCREEN_HEIGHT);

      int player_frame_index = 0;
      int zombie_frame_index = 0;
      int frame_iteration_counter = 0;

      //boolean to determine whether the endscreen should be shown
      bool has_ended = false;

      //While game has not quit
      while (!quit) {

        //if player is on the endscreen
        if (has_ended) {
          while (SDL_PollEvent(&sdl_event)) {
            if (sdl_event.type == SDL_QUIT) {
                quit = true;
            } else if (sdl_event.type == SDL_KEYDOWN) {
              switch(sdl_event.key.keysym.sym) {
                case SDLK_r:
                  //restart the game if r is pressed
                  has_ended = false;
                  resetGame();
                  break;
              }
            }           
          }
          showEndgameScreen();

        } else {
          while (SDL_PollEvent(&sdl_event)) {
            if (sdl_event.type == SDL_QUIT) {
              quit = true;
            }
            g_player.handleEvent(sdl_event);

          }
        
          //spawn enemies
          randomSpawnEnemy();

          //Move the player and all enemies
          g_player.move(SCREEN_WIDTH, SCREEN_HEIGHT);
          auto it = enemies.begin();
          while (it != enemies.end()) {
            (*it)->move(g_player, enemies);
            it++;
          }

          g_player.adjustAimAccuracy();

          g_background_texture.render(0, 0, g_renderer.get());

          //handle all shots 
          it = enemies.begin();
          while (it != enemies.end()) {
              handleShot(g_player, **it);
              if ((*it)->isDead()) {
                  it = enemies.erase(it);
                  g_score++;
              } else {
                  it++;
              }
          }

          enemyAttackHandler();

          if (frame_iteration_counter++ % FRAME_CHANGE_ITERATIONS == FRAME_CHANGE_ITERATIONS - 1) {
            frame_iteration_counter = 0;
          
            //if the player is moving forward advance the frame
            if (g_player.getMovingState() == Player::MOVING_FORWARD) {
              //if the next frame is ready to be shown
              player_frame_index = (player_frame_index + 1)%PLAYER_FRAMES_COUNT;
            } else if (g_player.getMovingState() == Player::MOVING_BACKWARD) {
              //if the previous frame is ready to be shown
              player_frame_index--;
              
              //if index is negative then circle around
              if (player_frame_index < 0) {
                player_frame_index += PLAYER_FRAMES_COUNT;
              }
            }
            zombie_frame_index = (zombie_frame_index + 1)%ENEMY_FRAMES_COUNT;
          }

          //check is the player has died
          if (g_player.isDead()) {
            has_ended = true;
          }

          //render player and all enemies
          g_player.render(g_renderer.get(), g_player_texture[player_frame_index]);
          it = enemies.begin();
          while (it != enemies.end()) {
            if (!(*it)->is_super) {
              (*it)->render(g_renderer.get(), g_enemy_texture[zombie_frame_index]);
            } else {
              (*it)->render(g_renderer.get(), g_super_enemy_texture[zombie_frame_index]);
            }
            it++;
          }

          //render score
          renderScore(SCREEN_WIDTH, 0);

          //render resource bar
          r_bar.render(g_player, g_renderer.get());
          
          //update screen
          SDL_RenderPresent(g_renderer.get());
        }
      }
    }
  }
  //close the game
  close();

  return 0;
}

bool initialiseGame() {
  //initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << SDL_GetError();
    return false;
  } else {
    //try to enable linear filtering
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
      std::cerr << "linear filtering not available";
    }

    window_ptr window(SDL_CreateWindow("Zombie Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN), &SDL_DestroyWindow);
    g_window = std::move(window);

    if (!g_window) {
      std::cerr << SDL_GetError();
      return false;
    } else {
      //create SDL_Renderer for the window
      renderer_ptr renderer {SDL_CreateRenderer(g_window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC), &SDL_DestroyRenderer};
      g_renderer = std::move(renderer);
      if (!g_renderer) {
        std::cerr << SDL_GetError();
        return false;
      } else {
        int imgFlags = IMG_INIT_PNG;
        if (IMG_Init(imgFlags) & imgFlags != imgFlags) {
          std::cerr << IMG_GetError();
          return false;
        }
        if (TTF_Init() == -1) {
          std::cerr << TTF_GetError();
          return false;
        }
      }
    }
  }

  return true;
}

bool loadMedia() {
  //Loading success flag, this method allows us to see which images are not being loaded correctly
  bool success = true;

  //load player textures
  if (!g_player_texture[0].loadFromFile("./pixel_art/separate_pics/sprite_soldier0.png", g_renderer.get(), Player::WIDTH)) {
    std::cerr << "Failed to load player texture 0\n";
    success = false;
  }

  
  if (!g_player_texture[1].loadFromFile("./pixel_art/separate_pics/sprite_soldier1.png", g_renderer.get(), Player::WIDTH)) {
    std::cerr << "Failed to load player texture 1\n";
    success = false;
  }

  
  if (!g_player_texture[2].loadFromFile("./pixel_art/separate_pics/sprite_soldier2.png", g_renderer.get(), Player::WIDTH)) {
    std::cerr << "Failed to load player texture 2\n";
    success = false;
  }

  
  if (!g_player_texture[3].loadFromFile("./pixel_art/separate_pics/sprite_soldier3.png", g_renderer.get(), Player::WIDTH)) {
    std::cerr << "Failed to load player texture 3\n";
    success = false;
  }

  //load nnemy textures
  if (!g_enemy_texture[0].loadFromFile("./pixel_art/separate_pics/zombie_sprite0.png", g_renderer.get(), Enemy::ENEMY_WIDTH)) {
    std::cerr << "Failed to load Enemy texture 0\n";
    success = false;    
  }

  
  if (!g_enemy_texture[1].loadFromFile("./pixel_art/separate_pics/zombie_sprite1.png", g_renderer.get(), Enemy::ENEMY_WIDTH)) {
    std::cerr << "Failed to load Enemy texture 1\n";
    success = false;
  }
  
  if (!g_enemy_texture[2].loadFromFile("./pixel_art/separate_pics/zombie_sprite2.png", g_renderer.get(), Enemy::ENEMY_WIDTH)) {
    std::cerr << "Failed to load Enemy texture 2\n";
    success = false;    
  }
  
  if (!g_enemy_texture[3].loadFromFile("./pixel_art/separate_pics/zombie_sprite3.png", g_renderer.get(), Enemy::ENEMY_WIDTH)) {
    std::cerr << "Failed to load Enemy texture 3\n";
    success = false;    
  }

  //load super Enemy
  if (!g_super_enemy_texture[0].loadFromFile("./pixel_art/separate_pics/super_zombie_sprite_0.png", g_renderer.get(), Enemy::ENEMY_WIDTH)) {
    std::cerr << "Failed to load super Enemy texture 0\n";
    success = false;    
  }

  
  if (!g_super_enemy_texture[1].loadFromFile("./pixel_art/separate_pics/super_zombie_sprite_1.png", g_renderer.get(), Enemy::ENEMY_WIDTH)) {
    std::cerr << "Failed to load super Enemy texture 1\n";
    success = false;
  }
  
  if (!g_super_enemy_texture[2].loadFromFile("./pixel_art/separate_pics/super_zombie_sprite_2.png", g_renderer.get(), Enemy::ENEMY_WIDTH)) {
    std::cerr << "Failed to load super Enemy texture 2\n";
    success = false;    
  }
  
  if (!g_super_enemy_texture[3].loadFromFile("./pixel_art/separate_pics/super_zombie_sprite_3.png", g_renderer.get(), Enemy::ENEMY_WIDTH)) {
    std::cerr << "Failed to load super Enemy texture 3\n";
    success = false;    
  }

  font_ptr font(TTF_OpenFont("fonts/comic.ttf", 30), &TTF_CloseFont);
  g_font = std::move(font);
  if (!g_font) {
    std::cerr << TTF_GetError();
    success = false;
  }

  if (!g_background_texture.loadFromFile("./pixel_art/grass_background.png", g_renderer.get(), SCREEN_WIDTH)) {
    std::cerr << SDL_GetError();
    success = false;
  }
    
  return success;
}

void close() {
  IMG_Quit();
  SDL_Quit();
  TTF_Quit();
}

void randomSpawnEnemy() {
  int chance_inverse = 1000;
  int chance_super_enemy = 3;
  int random_num = random(chance_inverse);
  int random_num_for_super = random(chance_super_enemy);
  if (random_num == 0) {
    if (random_num_for_super == 0) {
      //smaller chance to spawn super enemy
      enemies.emplace_front(new SuperEnemy(0, random(SCREEN_HEIGHT)));
    } else {
      enemies.emplace_front(new Enemy(0, random(SCREEN_HEIGHT)));
    }
  }
}

void enemyAttackHandler() {
  int reg_dmg = 1;
  int spc_dmg = 3;
  auto it = enemies.begin();
  while (it != enemies.end()) {
    if (findDistance(g_player.getVisibleCentre(), (*it)->getCentre()) < Enemy::ENEMY_WIDTH * 0.75) {
      //choose what damage depending on the type of enemy attacking the player
      g_player.takeDamage((**it).is_super ? spc_dmg : reg_dmg);
    }
    it++;
  }
}

void renderScore(int x_pos, int y_pos) {
  TextureWrapper scoreTexture;
  std::string number = std::to_string(g_score);
  std::string score_str = "Score: ";
  SDL_Color color = {0,0,0};
  if (!scoreTexture.loadTTFTexture(score_str + number, g_font.get(), color, g_renderer.get())) {
    std::cerr << "error loading score texture\n";
  }

  scoreTexture.render(x_pos - 2*scoreTexture.getWidth(), y_pos, g_renderer.get());
  
}

void showEndgameScreen() {
  TextureWrapper endGameTexture;
  std::string message = "Game Over! Press R to replay";
  SDL_Color color = {0xff, 0x0, 0x0};
  if (!endGameTexture.loadTTFTexture(message, g_font.get(), color, g_renderer.get())) {
    std::cerr << "error loading end game texture";
  }
  endGameTexture.render(SCREEN_WIDTH/2 - endGameTexture.getWidth()/2, SCREEN_HEIGHT/2 - endGameTexture.getHeight()/2, g_renderer.get());
  SDL_RenderPresent(g_renderer.get());
}

void resetGame() {
  g_score = 0;
  Player new_player{SCREEN_WIDTH, SCREEN_HEIGHT/2};
  g_player = new_player;
  enemies.clear();
}
