#include "Game.h"
#include "PerlinNoise.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {

    Game game;

    if (game.init()) {
        game.run();
    }

    game.clean();

    return 0;
}
