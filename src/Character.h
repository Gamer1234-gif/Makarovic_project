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

    void drawCircle(SDL_Renderer* ren, int centerX, int centerY, int radius) const {
        for (int w = -radius; w < radius; w++) {
            for (int h = -radius; h < radius; h++) {
                if (w * w + h * h >= radius * radius - radius && w * w + h * h <= radius * radius + radius) {
                    SDL_RenderDrawPoint(ren, centerX + w, centerY + h);
                }
            }
        }
    }

    bool nearbyEnemy(const auto enemy) const {
        float distX = x - enemy.x;
        float distY = y - enemy.y;
        if (distX * distX + distY * distY < 22500) { // 150 pixels radius, 150^2 = 22500
            return true;
        }

        return false;
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
    bool nearbyEnemy(const std::vector<Enemy>& enemies) const {
        for (const auto& enemy : enemies) {
            if (&enemy != this) {
                float distX = x - enemy.x;
                float distY = y - enemy.y;
                if (distX * distX + distY * distY < 22500) { // 150 pixels radius, 150^2 = 22500
                    return true;
                }
            }
        }
        return false;
    }

    void setHasTrash(bool value) {
        hasTrash = value;
    }

    bool getHasTrash() const {
        return hasTrash;
    }

    void giveTrash() {
        int chance = rand() % 4;
        if (chance == 2) {
            hasTrash = true; // 25% chance to spawn with trash
        }
    }

    int getDirX() const {
        return dirX;
    }
    int getDirY() const {
        return dirY;
    }

    bool checkRight(const std::vector<std::vector<int>>& grid) const;
    bool checkLeft(const std::vector<std::vector<int>>& grid) const;
    bool checkUp(const std::vector<std::vector<int>>& grid) const;
    bool checkDown(const std::vector<std::vector<int>>& grid) const;
    void Enemy_timer(float deltaTime) {
        timer += deltaTime;
        if (timer >= 30.0f) {
            giveTrash();
            timer = 0.0f;
        }
    }

private:
    float speed = 15.0f;
    int dirX = 0;
    int dirY = 0;
    bool hasTrash = false;
    float timer = 0.0f;
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

    void SpawnFromEnemy(
        const Enemy& enemy, const std::vector<std::vector<int>>& grid, int windowWidth, int windowHeight
    ) {
        int gridX = (enemy.x + 10) / 30;
        int gridY = (enemy.y + 10) / 30;

        std::vector<std::pair<int, int>> directions;
        if (gridY - 1 >= 0 && grid[gridY - 1][gridX] == 0) {
            directions.emplace_back(0, -1); // Up
        }
        if (gridY + 1 < (int)grid.size() && grid[gridY + 1][gridX] == 0) {
            directions.emplace_back(0, 1); // Down
        }
        if (gridX - 1 >= 0 && grid[gridY][gridX - 1] == 0) {
            directions.emplace_back(-1, 0); // Left
        }
        if (gridX + 1 < (int)grid[gridY].size() && grid[gridY][gridX + 1] == 0) {
            directions.emplace_back(1, 0); // Right
        }

        if (!directions.empty()) {
            auto [dx, dy] = directions[rand() % directions.size()];
            x = enemy.x + dx * 30;
            y = enemy.y + dy * 30;
            if (x < windowWidth - 20 && y < windowHeight - 20) {
                std::cout << "Trash spawned from enemy at: (" << x << ", " << y << ")\n";
                dirX = dx;
                dirY = dy;
                return;
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