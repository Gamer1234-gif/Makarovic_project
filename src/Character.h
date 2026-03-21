#pragma once
#include "PerlinNoise.hpp"
#include <SDL2/SDL.h>
#include <ctime>
#include <iostream>
#include <vector>

class Character {
public:
    virtual void update(int windowWidth, int windowHeight) = 0;
    virtual void render(SDL_Renderer* ren) const = 0;
    virtual void Spawn(const std::vector<std::vector<int>>& grid, int squareSize) = 0;

    virtual ~Character() {
    }

    SDL_Rect getRect() const {
        return SDL_Rect{(int)x, (int)y, 20, 20};
    };

    float x, y;
};

class Player : public Character {
public:
    void handleInput(const SDL_Event& e);
    void update(int windowWidth, int windowHeight) override;
    void render(SDL_Renderer* ren) const override;
    void Spawn(const std::vector<std::vector<int>>& grid, int squareSize) override {
        x = 100;
        y = 100;
    };
    void Boundaries(int windowWidth, int windowHeight) {
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > windowWidth - 20) x = windowWidth - 20;
        if (y > windowHeight - 20) y = windowHeight - 20;
    };

private:
    float speed = 5.0f;
    bool up = false, down = false, left = false, right = false;
    int maxHeight;
    int maxWidth;
};

class Enemy : public Character {
public:
    void update(int windowWidth, int windowHeight) override;
    void render(SDL_Renderer* ren) const override;
    void Spawn(const std::vector<std::vector<int>>& grid, int squareSize) override {
        for (size_t i = 0; i < grid.size(); ++i) {
            for (size_t j = 0; j < grid[i].size(); ++j) {
                int chance = rand() % 1000;             // Random chance to spawn on land
                if (chance == 998 && grid[i][j] == 1) { // 0.5% chance to spawn on land
                    if (grid[i][j] == 1) {              // Land
                        x = j * squareSize + 5;         // Center in the cell
                        y = i * squareSize + 5;
                        std::cout << "Enemy spawned at: (" << x << ", " << y << ")\n";
                        return;
                    }
                }
            }
        }
    }
};