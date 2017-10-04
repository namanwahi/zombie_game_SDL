cmake_minimum_required(VERSION 3.8)
project(zombie_game_SDL)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        Enemy.cpp
        Enemy.h
        geometry_utils.cpp
        geometry_utils.h
        line.h
        main.cpp
        player.cpp
        player.h
        rand_utils.cpp
        rand_utils.h
        resource_bar.cpp
        resource_bar.h
        shot_handler.h
        texture_wrapper.cpp
        texture_wrapper.h)

add_executable(zombie_game_SDL ${SOURCE_FILES})

INCLUDE(FindPkgConfig)


PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})
