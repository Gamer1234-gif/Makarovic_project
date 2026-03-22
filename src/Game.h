#pragma once
#include "Character.h"
#include "PerlinNoise.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

class Game {
public:
    bool init();
    void run();
    void handleEvents();
    void update();
    void render();
    void clean();

private:
    SDL_Window* win = nullptr;
    SDL_Renderer* ren = nullptr;
    bool running = false;

    static const int GRID_SIZE = 75;
    static const int SQUARE_SIZE = 30;
    int windowWidth = 1920;
    int windowHeight = 1080;

    std::vector<std::vector<int>> grid;

    static constexpr double frequency = 0.02;  // lower = bigger continents
    static constexpr double threshold = 0.525; // higher = less land
    PerlinNoise noise{16305};                  // Fixed seed for consistent results

    Player player;
    std::vector<Enemy> enemies;
    std::vector<Friend> friends;
    std::vector<Trash> trash;

    TTF_Font* font = nullptr;

    int score = 0;
    int enemiesRemaining = 0;
    int trashRemaining = 0;
    int friendsRemaining = 0;
};