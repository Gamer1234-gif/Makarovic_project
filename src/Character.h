#pragma once
#include "PerlinNoise.hpp"
#include <SDL2/SDL.h>
#include <ctime>
#include <iostream>
#include <vector>

class Boat;   // Forward declaration
class Trash;  // Forward declaration
class Enemy;  // Forward declaration
class Friend; // Forward declaration

class Character {
public:
    virtual void update(int windowWidth, int windowHeight, int grid[75][75], float deltaTime) = 0;
    virtual void render(SDL_Renderer* ren) const = 0;
    virtual void Spawn(int grid[75][75], int squareSize, int windowWidth, int windowHeight) = 0;

    virtual ~Character() {
    }

    SDL_Rect getRect() const {
        return SDL_Rect{(int)x, (int)y, 20, 20};
    };

    void setSpeed(float newSpeed) {
        speed = newSpeed;
    }

    float x, y, timer = 0.0f, speed;
};

class Player {
public:
    void handleInput(const SDL_Event& e);
    void boatExitInput(const SDL_Event& e, int grid[75][75], const Boat& boat);
    void update(int windowWidth, int windowHeight, int grid[75][75], const Boat& boat, float deltaTime);
    void render(SDL_Renderer* ren) const;
    void Spawn(int grid[75][75], int squareSize, int windowWidth, int windowHeight) {
        x = 100;
        y = 100;
        if (grid[3][3] == 0) { // Spawn on water
            x = 3 * squareSize + 5;
            y = 3 * squareSize + 5;
            inBoat = true;
            std::cout << "Player spawned in boat at: (" << x << ", " << y << ")\n";
        }
    };
    void Boundaries(int windowWidth, int windowHeight) {
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > windowWidth - 20) x = (float)(windowWidth - 20);
        if (y > windowHeight - 20) y = (float)(windowHeight - 20);
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
        return (int)x;
    }
    int getY() const {
        return (int)y;
    }
    void setX(int newX) {
        x = (float)newX;
    }
    void setY(int newY) {
        y = (float)newY;
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

    int nearbyEnemycount(const std::vector<Enemy>& enemies) const;
    bool nearbyEnemyRender(const Enemy& enemy) const;

    int nearbyFriend(const std::vector<Friend>& friends) const;

    void setSpeed(float newSpeed) {
        speed = newSpeed;
    }

    void resetMovement() {
        moveUp = false;
        moveDown = false;
        moveLeft = false;
        moveRight = false;
    }

private:
    float x, y;
    bool inBoat = false, moveUp = false, moveDown = false, moveLeft = false, moveRight = false;
    float speed;
};


class Enemy : public Character {
public:
    void update(int windowWidth, int windowHeight, int grid[75][75], float deltaTime) override;
    void render(SDL_Renderer* ren) const override;
    void Spawn(int grid[75][75], int squareSize, int windowWidth, int windowHeight) override {
        while (true) {
            size_t i = rand() % 75;
            size_t j = rand() % 75;
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
    int nearbyEnemy(const std::vector<Enemy>& enemies) const {
        int count = 0;

        for (const auto& enemy : enemies) {
            if (&enemy != this) {
                float distX = x - enemy.x;
                float distY = y - enemy.y;
                if (distX * distX + distY * distY < 22500) { // 150 pixels radius, 150^2 = 22500
                    count++;
                }
            }
        }
        return count;
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
            std::cout << "Enemy at (" << x << ", " << y << ") given trash.\n";
            hasTrash = true; // 25% chance to spawn with trash
        }
    }

    int getDirX() const {
        return dirX;
    }
    int getDirY() const {
        return dirY;
    }

    bool checkRight(int grid[75][75]) const;
    bool checkLeft(int grid[75][75]) const;
    bool checkUp(int grid[75][75]) const;
    bool checkDown(int grid[75][75]) const;
    void Enemy_timer(float deltaTime) {
        timerTrash += deltaTime;
        if (timerTrash >= 30.0f) {
            giveTrash();
            timerTrash = 0.0f;
        }
    }

    void updateTrashDropTimer(float deltaTime) {
        trashDropTimer += deltaTime;
    }

    bool canDropTrash() const {
        return trashDropTimer >= 3.0f; // Can drop trash every 3 seconds
    }

    void resetTrashDropTimer() {
        trashDropTimer = 0.0f;
    }

    void setSpeed(float newSpeed) {
        speed = newSpeed;
    }

    float getX() const {
        return x;
    }

    float getY() const {
        return y;
    }

    // Alert system: returns true if enemy sees player
    bool canSeePlayer(float playerX, float playerY) const {
        float distX = x - playerX;
        float distY = y - playerY;
        return (distX * distX + distY * distY) < 22500; // 150 pixels
    }

    void setAlerted(bool value) {
        isAlerted = value;
    }

    bool getIsAlerted() const {
        return isAlerted;
    }

    // Alert nearby enemies and get them to chase player
    void alertNearbyEnemies(std::vector<Enemy>& enemies, float playerX, float playerY) {
        for (auto& enemy : enemies) {
            if (&enemy != this) {
                float distX = x - enemy.x;
                float distY = y - enemy.y;
                if (distX * distX + distY * distY < 22500) { // 150 pixels radius
                    enemy.setAlerted(true);
                }
            }
        }
    }

    void moveTowardsPlayer(float playerX, float playerY) {
        if (isAlerted) {
            float distX = playerX - x;
            float distY = playerY - y;
            float length = sqrt(distX * distX + distY * distY);
            if (distX > 0)
                dirX = 1;
            else if (distX < 0)
                dirX = -1;
            else
                dirX = 0;

            if (distY > 0)
                dirY = 1;
            else if (distY < 0)
                dirY = -1;
            else
                dirY = 0;
        }
    }

private:
    float speed;
    int dirX = 0;
    int dirY = 0;
    bool hasTrash = false;
    bool isAlerted = false;
    float timerTrash = 0.0f, timerMove = 0.0f;
    float trashDropTimer = 0.0f;
};

class Friend : public Character {
public:
    void update(int windowWidth, int windowHeight, int grid[75][75], float deltaTime) override;
    void render(SDL_Renderer* ren) const override;
    void Spawn(int grid[75][75], int squareSize, int windowWidth, int windowHeight) override {
        while (true) {
            size_t i = rand() % 75;
            size_t j = rand() % 75;
            x = j * squareSize + 5;
            y = i * squareSize + 5;
            if (x < windowWidth - 20 && y < windowHeight - 20) {
                std::cout << "Friend spawned at: (" << x << ", " << y << ")\n";
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

    void setSpeed(float newSpeed) {
        speed = newSpeed;
    }

    int nearbyFriend(const std::vector<Friend>& friends) const {
        int stev = 0;

        for (const auto& friend_ : friends) {
            if (&friend_ != this) {
                float distX = x - friend_.x;
                float distY = y - friend_.y;
                if (distX * distX + distY * distY < 22500) { // 150 pixels radius, 150^2 = 22500
                    stev++;
                }
            }
        }

        return stev;
    }

    int nearbyPlayer(const Player& player) const {
        float distX = x - player.getX();
        float distY = y - player.getY();
        if (distX * distX + distY * distY < 22500) { // 150 pixels radius, 150^2 = 22500
            return 1;
        }

        return 0;
    }


private:
    float speed;
    int dirX = 0;
    int dirY = 0;
};

class Trash : public Character {
public:
    void update(int windowWidth, int windowHeight, int grid[75][75], float deltaTime) override;
    void render(SDL_Renderer* ren) const override;
    void Spawn(int grid[75][75], int squareSize, int windowWidth, int windowHeight) override {
        while (true) {
            size_t i = rand() % 75;
            size_t j = rand() % 75;
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

    void SpawnFromEnemy(const Enemy& enemy, int grid[75][75], int windowWidth, int windowHeight) {
        int gridX = (enemy.x + 10) / 30;
        int gridY = (enemy.y + 10) / 30;

        // Validate initial position
        if (gridY < 0 || gridY >= 75 || gridX < 0 || gridX >= 75) {
            return; // Invalid spawn position
        }

        std::vector<std::pair<int, int>> directions;
        if (gridY - 2 >= 0 && gridX < 75 && grid[gridY - 2][gridX] == 0) {
            directions.emplace_back(0, -1); // Up
        }
        if (gridY + 2 < 75 && gridX < 75 && grid[gridY + 2][gridX] == 0) {
            directions.emplace_back(0, 1); // Down
        }
        if (gridX - 2 >= 0 && grid[gridY][gridX - 2] == 0) {
            directions.emplace_back(-1, 0); // Left
        }
        if (gridX + 2 < 75 && grid[gridY][gridX + 2] == 0) {
            directions.emplace_back(1, 0); // Right
        }

        if (!directions.empty()) {
            auto [dx, dy] = directions[rand() % directions.size()];
            x = enemy.x + dx * 30 + 15;
            y = enemy.y + dy * 30 + 15;
            if (x < windowWidth - 20 && y < windowHeight - 20) {
                std::cout << "Trash spawned from enemy at: (" << x << ", " << y << ")\n";
                dirX = dx;
                dirY = dy;
                return;
            }
        }
    }

    void setSpeed(float newSpeed) {
        speed = newSpeed;
    }

    void renderAllTrash(SDL_Renderer* ren, const std::vector<Trash>& trashList) const {
        for (const auto& t : trashList) {
            t.render(ren);
        }
    }

private:
    float speed;
    int dirX = 0;
    int dirY = 1;
};

class Boat {
public:
    void handleInput(const SDL_Event& e);
    void update(int windowWidth, int windowHeight, int grid[75][75], float deltaTime);
    void render(SDL_Renderer* ren) const;
    bool check_up(int grid[75][75], int i, int j) const;
    bool check_down(int grid[75][75], int i, int j) const;
    bool check_left(int grid[75][75], int i, int j) const;
    bool check_right(int grid[75][75], int i, int j) const;
    void Spawn(int grid[75][75], int squareSize, int windowWidth, int windowHeight) {
        while (true) {
            size_t i = rand() % 75;
            size_t j = rand() % 75;
            if (grid[i][j] == 0 &&
                (check_up(grid, i - 1, j) || check_left(grid, i, j - 1) || check_down(grid, i + 1, j) ||
                 check_right(grid, i, j + 1)) &&
                grid[i - 1][j] == 1) { // Spawn on water
                x = (float)(j * squareSize + 5);
                y = (float)(i * squareSize + 5);
                if (x < windowWidth - 20 && y < windowHeight - 20) {
                    std::cout << "Boat spawned at: (" << x << ", " << y << ")\n";
                    return;
                }
            }
        }
    }
    void SpawnInOcean(float pl_x, float pl_y) {
        this->x = pl_x;
        this->y = pl_y;
    }

    SDL_Rect getRect() const {
        return SDL_Rect{(int)x, (int)y, 20, 20};
    };
    int getX() const {
        return (int)x;
    }
    int getY() const {
        return (int)y;
    }
    void setX(int newX) {
        x = (float)newX;
    }
    void setY(int newY) {
        y = (float)newY;
    }

    void setplayerInBoat(bool value) {
        playerInBoat = value;
        if (!value) {
            // Clear boat inputs when player exits
            moveUp = false;
            moveDown = false;
            moveLeft = false;
            moveRight = false;
        }
    }

    bool getplayerInBoat() const {
        return playerInBoat;
    }

    void setSpeed(float newSpeed) {
        speed = newSpeed;
    }

    void resetMovement() {
        moveUp = false;
        moveDown = false;
        moveLeft = false;
        moveRight = false;
    }

private:
    float speed;
    bool moveUp = false, moveDown = false, moveLeft = false, moveRight = false, playerInBoat = false;
    float x, y;
};