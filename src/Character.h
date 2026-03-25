#pragma once
#include "PerlinNoise.hpp"
#include <SDL2/SDL.h>
#include <ctime>
#include <iostream>
#include <vector>

class Boat; // Forward declaration

class Character {
public:
    virtual void update(
        int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, float deltaTime
    ) = 0;
    virtual void render(SDL_Renderer* ren) const = 0;
    virtual void Spawn(
        const std::vector<std::vector<int>>& grid, int squareSize, int windowWidth, int windowHeight
    ) = 0;

    virtual ~Character() {
    }

    SDL_Rect getRect() const {
        return SDL_Rect{(int)x, (int)y, 20, 20};
    };

    float x, y, timer = 0.0f;
};

class Player {
public:
    void handleInput(const SDL_Event& e);
    void boatExitInput(const SDL_Event& e, const std::vector<std::vector<int>>& grid, const Boat& boat);
    void update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, const Boat& boat);
    void render(SDL_Renderer* ren) const;
    void Spawn(const std::vector<std::vector<int>>& grid, int squareSize, int windowWidth, int windowHeight) {
        x = 100;
        y = 100;
    };
    void Boundaries(int windowWidth, int windowHeight) {
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > windowWidth - 20) x = windowWidth - 20;
        if (y > windowHeight - 20) y = windowHeight - 20;
    };
    SDL_Rect getRect() const {
        return SDL_Rect{(int)x, (int)y, 20, 20};
    };
    void setInBoat(bool value) {
        inBoat = value;
    }
    bool getInBoat() {
        return inBoat;
    }
    int getX() const {
        return x;
    }
    int getY() const {
        return y;
    }
    void setX(int newX) {
        x = newX;
    }
    void setY(int newY) {
        y = newY;
    }

private:
    int x, y;
    bool inBoat = false, moveUp = false, moveDown = false, moveLeft = false, moveRight = false;
    float speed = 5.0f;
};

class Enemy : public Character {
public:
    void update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, float deltaTime) override;
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
    void update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, float deltaTime) override;
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
    void update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, float deltaTime) override;
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
                    // Start with a random initial movement direction
                    int dir = rand() % 4;
                    if (dir == 0) {
                        dirX = 0;
                        dirY = -1;
                    } else if (dir == 1) {
                        dirX = 0;
                        dirY = 1;
                    } else if (dir == 2) {
                        dirX = -1;
                        dirY = 0;
                    } else {
                        dirX = 1;
                        dirY = 0;
                    }
                    return;
                }
            }
        }
    }

private:
    float speed = 20.0f;
    int dirX = 0;
    int dirY = 1;
};

class Boat {
public:
    void handleInput(const SDL_Event& e);
    void update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid);
    void render(SDL_Renderer* ren) const;
    bool check_up(const std::vector<std::vector<int>>& grid, int i, int j) const;
    bool check_down(const std::vector<std::vector<int>>& grid, int i, int j) const;
    bool check_left(const std::vector<std::vector<int>>& grid, int i, int j) const;
    bool check_right(const std::vector<std::vector<int>>& grid, int i, int j) const;
    void Spawn(const std::vector<std::vector<int>>& grid, int squareSize, int windowWidth, int windowHeight) {
        while (true) {
            size_t i = rand() % grid.size();
            size_t j = rand() % grid[i].size();
            if (grid[i][j] == 0 &&
                (check_up(grid, i - 1, j) || check_left(grid, i, j - 1) || check_down(grid, i + 1, j) ||
                 check_right(grid, i, j + 1)) &&
                grid[i - 1][j] == 1) { // Spawn on water
                x = j * squareSize + 5;
                y = i * squareSize + 5;
                if (x < windowWidth - 20 && y < windowHeight - 20) {
                    std::cout << "Boat spawned at: (" << x << ", " << y << ")\n";
                    return;
                }
            }
        }
    }
    SDL_Rect getRect() const {
        return SDL_Rect{(int)x, (int)y, 20, 20};
    };
    int getX() const {
        return x;
    }
    int getY() const {
        return y;
    }
    void setX(int newX) {
        x = newX;
    }
    void setY(int newY) {
        y = newY;
    }

    void setplayerInBoat(bool value) {
        playerInBoat = value;
    }

    bool getplayerInBoat() const {
        return playerInBoat;
    }

private:
    float speed = 5.0f;
    bool moveUp = false, moveDown = false, moveLeft = false, moveRight = false, playerInBoat = false;
    int x, y;
};