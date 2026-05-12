#include "Character.h"
#include <iostream>

void Player::handleInput(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_w: moveUp = true; break;
        case SDLK_s: moveDown = true; break;
        case SDLK_a: moveLeft = true; break;
        case SDLK_d: moveRight = true; break;
        }
    }

    if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
        case SDLK_w: moveUp = false; break;
        case SDLK_s: moveDown = false; break;
        case SDLK_a: moveLeft = false; break;
        case SDLK_d: moveRight = false; break;
        }
    }
}

void Player::boatExitInput(const SDL_Event& e, int grid[75][75], const Boat& boat) {
    int boatI = (boat.getY() + 10) / 30; // Boat's grid Y position
    int boatJ = (boat.getX() + 10) / 30; // Boat's grid X position
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LSHIFT &&
        (boat.check_up(grid, boatI - 1, boatJ) || boat.check_down(grid, boatI + 1, boatJ) ||
         boat.check_left(grid, boatI, boatJ - 1) || boat.check_right(grid, boatI, boatJ + 1))) {
        inBoat = false;
    }
}

void Player::update(int windowWidth, int windowHeight, int grid[75][75], const Boat& boat, float deltaTime) {
    // Calculate boat's grid position
    int boatGridX = (boat.getX() + 10) / 30;
    int boatGridY = (boat.getY() + 10) / 30;

    float movement = speed * deltaTime;

    if (moveUp && y - movement >= 0) {
        int centerX = (int)(x + 10);
        int nextY = (int)((y - movement + 10) / 30);
        int gridX = centerX / 30;

        if (nextY >= 0 && nextY < 75 && gridX >= 0 && gridX < 75 &&
            (grid[nextY][gridX] == 1 || (nextY == boatGridY && gridX == boatGridX))) {
            y -= movement;
        }
    }
    if (moveDown && y + movement <= windowHeight - 20) {
        int centerX = (int)(x + 10);
        int nextY = (int)((y + movement + 10) / 30);
        int gridX = centerX / 30;

        if (nextY >= 0 && nextY < 75 && gridX >= 0 && gridX < 75 &&
            (grid[nextY][gridX] == 1 || (nextY == boatGridY && gridX == boatGridX))) {
            y += movement;
        }
    }
    if (moveLeft && x - movement >= 0) {
        int centerY = (int)(y + 10);
        int nextX = (int)((x - movement + 10) / 30);
        int gridY = centerY / 30;

        if (gridY >= 0 && gridY < 75 && nextX >= 0 && nextX < 75 &&
            (grid[gridY][nextX] == 1 || (gridY == boatGridY && nextX == boatGridX))) {
            x -= movement;
        }
    }
    if (moveRight && x + movement <= windowWidth - 20) {
        int centerY = (int)(y + 10);
        int nextX = (int)((x + movement + 10) / 30);
        int gridY = centerY / 30;

        if (gridY >= 0 && gridY < 75 && nextX >= 0 && nextX < 75 &&
            (grid[gridY][nextX] == 1 || (gridY == boatGridY && nextX == boatGridX))) {
            x += movement;
        }
    }
}

void Player::render(SDL_Renderer* ren) const {
    SDL_SetRenderDrawColor(ren, 255, 255, 0, 255); // Yellow for player
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 20, 20};
    SDL_RenderFillRect(ren, &rect);
    if (!inBoat) {
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 50); // white circle
        drawCircle(ren, x + 10, y + 10, 150);
    }
}

int Player::nearbyEnemycount(const std::vector<Enemy>& enemies) const {
    int stev = 0;

    for (const auto& enemy : enemies) {
        float distX = x - enemy.getX();
        float distY = y - enemy.getY();

        if (distX * distX + distY * distY < 22500) {
            stev++;
        }
    }

    return stev;
}

int Player::nearbyFriend(const std::vector<Friend>& friends) const {
    int stev = 0;

    for (const auto& friend_ : friends) {
        float distX = x - friend_.x;
        float distY = y - friend_.y;
        if (distX * distX + distY * distY < 22500) { // 150 pixels radius, 150^2 = 22500
            stev++;
        }
    }

    return stev;
}

bool Player::nearbyEnemyRender(const Enemy& enemy) const {
    float distX = x - enemy.getX();
    float distY = y - enemy.getY();

    return distX * distX + distY * distY < 22500;
}

void Enemy::update(int windowWidth, int windowHeight, int grid[75][75], float deltaTime) {
    timerMove += deltaTime;
    // Random change interval between 2 and 5 seconds
    float changeInterval = 2.0f + (rand() % 31) / 10.0f; // 2.0 to 5.0 seconds
    if (timerMove >= changeInterval) {
        timerMove = 0.0f;

        int gridX = (x + 10) / 30;
        int gridY = (y + 10) / 30;
        std::vector<std::pair<int, int>> directions;

        if (gridX >= 0 && gridX < 75 && gridY >= 0 && gridY < 75) {
            if (gridY - 1 >= 0 && grid[gridY - 1][gridX] == 1) {
                directions.emplace_back(0, -1); // Up
            }
            if (gridY + 1 < 75 && grid[gridY + 1][gridX] == 1) {
                directions.emplace_back(0, 1); // Down
            }
            if (gridX - 1 >= 0 && grid[gridY][gridX - 1] == 1) {
                directions.emplace_back(-1, 0); // Left
            }
            if (gridX + 1 < 75 && grid[gridY][gridX + 1] == 1) {
                directions.emplace_back(1, 0); // Right
            }
        }

        if (!directions.empty()) {
            auto [dx, dy] = directions[rand() % directions.size()];
            dirX = dx;
            dirY = dy;
        }
    }

    float newX = x + dirX * speed * deltaTime;
    float newY = y + dirY * speed * deltaTime;

    int newGridX = (newX + 10) / 30;
    int newGridY = (newY + 10) / 30;

    if (newGridY >= 0 && newGridY < 75 && newGridX >= 0 && newGridX < 75 && grid[newGridY][newGridX] == 1 &&
        newX >= 0 && newX <= windowWidth - 20 && newY >= 0 && newY <= windowHeight - 20) {
        x = newX;
        y = newY;
    } else {
        // bounce back via a new random direction immediately
        std::vector<std::pair<int, int>> validDirs;
        int gridX = (x + 10) / 30;
        int gridY = (y + 10) / 30;

        if (gridY > 0 && grid[gridY - 1][gridX] == 1) validDirs.emplace_back(0, -1);
        if (gridY < 75 - 1 && grid[gridY + 1][gridX] == 1) validDirs.emplace_back(0, 1);
        if (gridX > 0 && grid[gridY][gridX - 1] == 1) validDirs.emplace_back(-1, 0);
        if (gridX < 75 - 1 && grid[gridY][gridX + 1] == 1) validDirs.emplace_back(1, 0);

        if (!validDirs.empty()) {
            auto [dx, dy] = validDirs[rand() % validDirs.size()];
            dirX = dx;
            dirY = dy;
        }
    }
}



void Enemy::render(SDL_Renderer* ren) const {
    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); // Red for enemy
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 20, 20};
    SDL_RenderFillRect(ren, &rect);
}

bool Enemy::checkRight(int grid[75][75]) const {
    int gridX = (x + 10) / 30;
    int gridY = (y + 10) / 30;
    if (gridX < 75 - 1 && grid[gridY][gridX + 1] == 0) {
        return true;
    }
    return false;
}

bool Enemy::checkLeft(int grid[75][75]) const {
    int gridX = (x + 10) / 30;
    int gridY = (y + 10) / 30;
    if (gridX > 0 && grid[gridY][gridX - 1] == 0) {
        return true;
    }
    return false;
}

bool Enemy::checkUp(int grid[75][75]) const {
    int gridX = (x + 10) / 30;
    int gridY = (y + 10) / 30;
    if (gridY > 0 && grid[gridY - 1][gridX] == 0) {
        return true;
    }
    return false;
}

bool Enemy::checkDown(int grid[75][75]) const {
    int gridX = (x + 10) / 30;
    int gridY = (y + 10) / 30;
    if (gridY < 75 - 1 && grid[gridY + 1][gridX] == 0) {
        return true;
    }
    return false;
}

void Friend::update(int windowWidth, int windowHeight, int grid[75][75], float deltaTime) {
    timer += deltaTime;
    // Random change interval between 2 and 5 seconds
    float changeInterval = 2.0f + (rand() % 31) / 10.0f; // 2.0 to 5.0 seconds
    if (timer >= changeInterval) {
        timer = 0.0f;

        int gridX = (x + 10) / 30;
        int gridY = (y + 10) / 30;
        std::vector<std::pair<int, int>> directions;

        if (gridX >= 0 && gridX < 75 && gridY >= 0 && gridY < 75) {
            if (gridY - 2 >= 0) {
                directions.emplace_back(0, -1); // Up
            }
            if (gridY + 2 < 75) {
                directions.emplace_back(0, 1); // Down
            }
            if (gridX - 2 >= 0) {
                directions.emplace_back(-1, 0); // Left
            }
            if (gridX + 2 < 75) {
                directions.emplace_back(1, 0); // Right
            }
        }

        if (!directions.empty()) {
            auto [dx, dy] = directions[rand() % directions.size()];
            dirX = dx;
            dirY = dy;
        }
    }

    float newX = x + dirX * speed * deltaTime;
    float newY = y + dirY * speed * deltaTime;

    int newGridX = (newX + 10) / 30;
    int newGridY = (newY + 10) / 30;

    if (newGridY >= 0 && newGridY < 75 && newGridX >= 0 && newGridX < 75 && newX >= 0 && newX <= windowWidth - 20 &&
        newY >= 0 && newY <= windowHeight - 20) {
        x = newX;
        y = newY;
    } else {
        // bounce back via a new random direction immediately
        std::vector<std::pair<int, int>> validDirs;
        int gridX = (x + 10) / 30;
        int gridY = (y + 10) / 30;

        if (gridY > 0) validDirs.emplace_back(0, -1);
        if (gridY < 75 - 1) validDirs.emplace_back(0, 1);
        if (gridX > 0) validDirs.emplace_back(-1, 0);
        if (gridX < 75 - 1) validDirs.emplace_back(1, 0);

        if (!validDirs.empty()) {
            auto [dx, dy] = validDirs[rand() % validDirs.size()];
            dirX = dx;
            dirY = dy;
        }
    }
}

void Friend::render(SDL_Renderer* ren) const {
    SDL_SetRenderDrawColor(ren, 20, 255, 125, 255); // Green for friend
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 20, 20};
    SDL_RenderFillRect(ren, &rect);
}

void Trash::update(int windowWidth, int windowHeight, int grid[75][75], float deltaTime) {
    timer += deltaTime;
    // Random change interval between 2 and 5 seconds
    float changeInterval = 2.0f + (rand() % 31) / 10.0f; // 2.0 to 5.0 seconds
    if (timer >= changeInterval) {
        timer = 0.0f;

        int gridX = (x + 10) / 30;
        int gridY = (y + 10) / 30;
        std::vector<std::pair<int, int>> directions;

        if (gridX >= 0 && gridX < 75 && gridY >= 0 && gridY < 75) {
            if (gridY - 2 >= 0 && grid[gridY - 2][gridX] == 0) {
                directions.emplace_back(0, -1); // Up
            }
            if (gridY + 2 < 75 && grid[gridY + 2][gridX] == 0) {
                directions.emplace_back(0, 1); // Down
            }
            if (gridX - 2 >= 0 && grid[gridY][gridX - 2] == 0) {
                directions.emplace_back(-1, 0); // Left
            }
            if (gridX + 2 < 75 && grid[gridY][gridX + 2] == 0) {
                directions.emplace_back(1, 0); // Right
            }
        }

        if (!directions.empty()) {
            auto [dx, dy] = directions[rand() % directions.size()];
            dirX = dx;
            dirY = dy;
        }
    }

    // Continuous motion in current direction
    float newX = x + dirX * speed * deltaTime;
    float newY = y + dirY * speed * deltaTime;

    int newGridX = (newX + 10) / 30;
    int newGridY = (newY + 10) / 30;

    if (newGridY >= 0 && newGridY < 75 && newGridX >= 0 && newGridX < 75 && grid[newGridY][newGridX] == 0 &&
        newX >= 0 && newX <= windowWidth - 20 && newY >= 0 && newY <= windowHeight - 20) {
        x = newX;
        y = newY;
    } else {
        // bounce back via a new random direction immediately
        std::vector<std::pair<int, int>> validDirs;
        int gridX = (x + 10) / 30;
        int gridY = (y + 10) / 30;

        // Validate grid indices before accessing
        if (gridX >= 0 && gridX < 75 && gridY >= 0 && gridY < 75) {
            if (gridY > 0 && grid[gridY - 1][gridX] == 0) validDirs.emplace_back(0, -1);
            if (gridY < 75 - 1 && grid[gridY + 1][gridX] == 0) validDirs.emplace_back(0, 1);
            if (gridX > 0 && grid[gridY][gridX - 1] == 0) validDirs.emplace_back(-1, 0);
            if (gridX < 75 - 1 && grid[gridY][gridX + 1] == 0) validDirs.emplace_back(1, 0);
        }

        if (!validDirs.empty()) {
            auto [dx, dy] = validDirs[rand() % validDirs.size()];
            dirX = dx;
            dirY = dy;
        }
    }
}

void Trash::render(SDL_Renderer* ren) const {
    SDL_SetRenderDrawColor(ren, 100, 100, 100, 255); // Red for trash
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 20, 20};
    SDL_RenderFillRect(ren, &rect);
}

void Boat::handleInput(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_w: moveUp = true; break;
        case SDLK_s: moveDown = true; break;
        case SDLK_a: moveLeft = true; break;
        case SDLK_d: moveRight = true; break;
        }
    }

    if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
        case SDLK_w: moveUp = false; break;
        case SDLK_s: moveDown = false; break;
        case SDLK_a: moveLeft = false; break;
        case SDLK_d: moveRight = false; break;
        }
    }
}

void Boat::update(int windowWidth, int windowHeight, int grid[75][75], float deltaTime) {
    float movement = speed * deltaTime;
    int gridX = (int)((x + 10) / 30);
    int gridY = (int)((y + 10) / 30);

    if (moveUp && y - movement >= 0) {
        int nextY = (int)(((y - movement) + 10) / 30);
        if (nextY >= 0 && nextY < 75 && gridX >= 0 && gridX < 75 && grid[nextY][gridX] == 0) {
            y -= movement;
        }
    }

    if (moveDown && y + movement < windowHeight - 20) {
        int nextY = (int)(((y + movement) + 10) / 30);
        if (nextY >= 0 && nextY < 75 && gridX >= 0 && gridX < 75 && grid[nextY][gridX] == 0) {
            y += movement;
        }
    }
    if (moveLeft && x - movement >= 0) {
        int nextX = (int)(((x - movement) + 10) / 30);
        if (gridY >= 0 && gridY < 75 && nextX >= 0 && nextX < 75 && grid[gridY][nextX] == 0) {
            x -= movement;
        }
    }
    if (moveRight && x + movement < windowWidth - 20) {
        int nextX = (int)(((x + movement) + 10) / 30);
        if (gridY >= 0 && gridY < 75 && nextX >= 0 && nextX < 75 && grid[gridY][nextX] == 0) {
            x += movement;
        }
    }
}

void Boat::render(SDL_Renderer* ren) const {
    SDL_SetRenderDrawColor(ren, 150, 75, 0, 255); // Brown for boat
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 25, 25};
    SDL_RenderFillRect(ren, &rect);
}

bool Boat::check_up(int grid[75][75], int i, int j) const {
    if (i > 0 && grid[i][j] == 1) {
        return true;
    }
    return false;
}

bool Boat::check_down(int grid[75][75], int i, int j) const {
    if (i < 75 - 1 && grid[i][j] == 1) {
        return true;
    }
    return false;
}

bool Boat::check_left(int grid[75][75], int i, int j) const {
    if (j > 0 && grid[i][j] == 1) {
        return true;
    }
    return false;
}

bool Boat::check_right(int grid[75][75], int i, int j) const {
    if (j < 75 - 1 && grid[i][j] == 1) {
        return true;
    }
    return false;
}