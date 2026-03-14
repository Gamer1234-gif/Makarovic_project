#include "PerlinNoise.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

double frequency = 0.02;  // lower = bigger continents
double threshold = 0.525; // higher = less land

PerlinNoise noise(16305); // Fixed seed for consistent results

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    const int WINDOW_WIDTH = 1920;
    const int WINDOW_HEIGHT = 1080;

    SDL_Window* win = SDL_CreateWindow(
        "GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
    );
    if (!win) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // Generate 50x50 grid: 1 = land, 0 = sea (static checkerboard for testing)
    const int GRID_SIZE = 75;   // Smaller grid for bigger squares
    const int SQUARE_SIZE = 30; // 75 * 30 = 2250 pixels, fits nicely
    std::vector<std::vector<int>> grid(GRID_SIZE, std::vector<int>(GRID_SIZE));
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            double n = noise.noise(i * frequency, j * frequency);

            // Perlin returns roughly -1..1, normalize to 0..1
            n = (n + 1.0) / 2.0;

            // Convert to 0 or 1
            grid[i][j] = (n > threshold) ? 1 : 0;
        }
    }

    int land = 0;
    int sea = 0;

    // Count tiles safely using grid size
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int y = 0; y < GRID_SIZE; y++) {
            if (grid[x][y] == 1)
                land++;
            else
                sea++;
        }
    }

    std::cout << "Land: " << land << " Sea: " << sea << "\n";

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        // Draw grid of squares
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                SDL_Rect rect = {j * SQUARE_SIZE, i * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
                if (grid[i][j] == 1) {
                    // Land (green)
                    SDL_SetRenderDrawColor(ren, 50, 150, 50, 255);
                } else {
                    // Sea (blue)
                    SDL_SetRenderDrawColor(ren, 0, 100, 200, 255);
                }
                SDL_RenderFillRect(ren, &rect);
            }
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
