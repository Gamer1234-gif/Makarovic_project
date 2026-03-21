#include "Character.h"
#include "Game.h"
#include <iostream>


bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return false;
    }

    win = SDL_CreateWindow(
        "GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN
    );

    if (!win) return false;

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!ren) return false;

    // GRID GENERATION
    grid.resize(GRID_SIZE, std::vector<int>(GRID_SIZE));

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            double n = noise.noise(i * frequency, j * frequency);
            n = (n + 1.0) / 2.0;
            grid[i][j] = (n > threshold) ? 1 : 0;
        }
    }

    // Initialize player in the middle of the screen
    player.x = 1920 / 2 - 10; // Center horizontally, adjust for size
    player.y = 1080 / 2 - 10; // Center vertically

    // Initialize enemy in the middle of the top left quadrant
    enemy.x = 960 / 2 - 10; // Middle of 0-960
    enemy.y = 540 / 2 - 10; // Middle of 0-540

    running = true;
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        }
        player.handleInput(event);
    }
}

void Game::update() {
    player.update(windowWidth, windowHeight);
    player.Boundaries(windowWidth, windowHeight);
    enemy.update(windowWidth, windowHeight);
}

void Game::render() {
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid[i][j] == 1) {
                SDL_SetRenderDrawColor(ren, 0, 200, 0, 255);
            } else {
                SDL_SetRenderDrawColor(ren, 0, 0, 200, 255);
            }
            SDL_Rect cell{j * SQUARE_SIZE, i * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
            SDL_RenderFillRect(ren, &cell);
        }
    }

    player.render(ren);
    enemy.render(ren);

    SDL_RenderPresent(ren);
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}

void Game::clean() {
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}