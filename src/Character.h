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
    virtual void Spawn(
        const std::vector<std::vector<int>>& grid, int squareSize, int windowWidth, int windowHeight
    ) = 0;

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
    void boatExitInput(const SDL_Event& e);
    void update(int windowWidth, int windowHeight) override;
    void render(SDL_Renderer* ren) const override;
    void Spawn(const std::vector<std::vector<int>>& grid, int squareSize, int windowWidth, int windowHeight) override {
        x = 100;
        y = 100;
    };
    void Boundaries(int windowWidth, int windowHeight) {
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > windowWidth - 20) x = windowWidth - 20;
        if (y > windowHeight - 20) y = windowHeight - 20;
    };

    bool inBoat = false;

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
    void Spawn(const std::vector<std::vector<int>>& grid, int squareSize, int windowWidth, int windowHeight) override {
        while (true) {
            size_t i = rand() % grid.size();
            size_t j = rand() % grid[i].size();
            if (grid[i][j] == 1) { // Spawn on land
                x = j * squareSize + 5;
                y = i * squareSize + 5;
                if (x < windowWidth - 20 && y < windowHeight - 20) {
                    std::cout << "Enemy spawned at: (" << x << ", " << y << ")\n";
                    return;
                }
            }
        }
    }
};

class Friend : public Character {
public:
    void update(int windowWidth, int windowHeight) override;
    void render(SDL_Renderer* ren) const override;
    void Spawn(const std::vector<std::vector<int>>& grid, int squareSize, int windowWidth, int windowHeight) override {
        while (true) {
            size_t i = rand() % grid.size();
            size_t j = rand() % grid[i].size();
            if (grid[i][j] == 1) { // Spawn on land
                x = j * squareSize + 5;
                y = i * squareSize + 5;
                if (x < windowWidth - 20 && y < windowHeight - 20) {
                    std::cout << "Friend spawned at: (" << x << ", " << y << ")\n";
                    return;
                }
            }
        }
    }
};

class Trash : public Character {
public:
    void update(int windowWidth, int windowHeight) override;
    void render(SDL_Renderer* ren) const override;
    void Spawn(const std::vector<std::vector<int>>& grid, int squareSize, int windowWidth, int windowHeight) override {
        while (true) {
            size_t i = rand() % grid.size();
            size_t j = rand() % grid[i].size();
            if (grid[i][j] == 0) { // Spawn on water
                x = j * squareSize + 5;
                y = i * squareSize + 5;
                if (x < windowWidth - 20 && y < windowHeight - 20) {
                    std::cout << "Trash spawned at: (" << x << ", " << y << ")\n";
                    return;
                }
            }
        }
    }
};

class Boat : public Character {
public:
    void handleInput(const SDL_Event& e);
    void update(int windowWidth, int windowHeight) override;
    void render(SDL_Renderer* ren) const override;
    void Spawn(const std::vector<std::vector<int>>& grid, int squareSize, int windowWidth, int windowHeight) override {
        while (true) {
            size_t i = rand() % grid.size();
            size_t j = rand() % grid[i].size();
            if (grid[i][j] == 0) { // Spawn on water
                x = j * squareSize + 5;
                y = i * squareSize + 5;
                if (x < windowWidth - 20 && y < windowHeight - 20) {
                    std::cout << "Boat spawned at: (" << x << ", " << y << ")\n";
                    return;
                }
            }
        }
    }

private:
    float speed = 5.0f;
    bool up = false, down = false, left = false, right = false;
};