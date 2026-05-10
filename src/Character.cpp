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

void Player::boatExitInput(const SDL_Event& e, const std::vector<std::vector<int>>& grid, const Boat& boat) {
    int boatI = (boat.getY() + 10) / 30; // Boat's grid Y position
    int boatJ = (boat.getX() + 10) / 30; // Boat's grid X position
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LSHIFT &&
        (boat.check_up(grid, boatI - 1, boatJ) || boat.check_down(grid, boatI + 1, boatJ) ||
         boat.check_left(grid, boatI, boatJ - 1) || boat.check_right(grid, boatI, boatJ + 1))) {
        inBoat = false;
    }
}

void Player::update(
    int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, const Boat& boat, float deltaTime
) {
    // Calculate boat's grid position
    int boatGridX = (boat.getX() + 10) / 30;
    int boatGridY = (boat.getY() + 10) / 30;

    float movement = speed * deltaTime;

    if (moveUp && y - movement >= 0) {
        int centerX = (int)(x + 10);
        int nextY = (int)((y - movement + 10) / 30);
        int gridX = centerX / 30;

        if (nextY >= 0 && nextY < (int)grid.size() && gridX >= 0 && gridX < (int)grid[nextY].size() &&
            (grid[nextY][gridX] == 1 || (nextY == boatGridY && gridX == boatGridX))) {
            y -= movement;
        }
    }
    if (moveDown && y + movement <= windowHeight - 20) {
        int centerX = (int)(x + 10);
        int nextY = (int)((y + movement + 10) / 30);
        int gridX = centerX / 30;

        if (nextY >= 0 && nextY < (int)grid.size() && gridX >= 0 && gridX < (int)grid[nextY].size() &&
            (grid[nextY][gridX] == 1 || (nextY == boatGridY && gridX == boatGridX))) {
            y += movement;
        }
    }
    if (moveLeft && x - movement >= 0) {
        int centerY = (int)(y + 10);
        int nextX = (int)((x - movement + 10) / 30);
        int gridY = centerY / 30;

        if (gridY >= 0 && gridY < (int)grid.size() && nextX >= 0 && nextX < (int)grid[gridY].size() &&
            (grid[gridY][nextX] == 1 || (gridY == boatGridY && nextX == boatGridX))) {
            x -= movement;
        }
    }
    if (moveRight && x + movement <= windowWidth - 20) {
        int centerY = (int)(y + 10);
        int nextX = (int)((x + movement + 10) / 30);
        int gridY = centerY / 30;

        if (gridY >= 0 && gridY < (int)grid.size() && nextX >= 0 && nextX < (int)grid[gridY].size() &&
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

bool Player::nearbyEnemyRender(const Enemy& enemy) const {
    float distX = x - enemy.getX();
    float distY = y - enemy.getY();

    return distX * distX + distY * distY < 22500;
}

void Enemy::update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, float deltaTime) {
    timerMove += deltaTime;
    // Random change interval between 2 and 5 seconds
    float changeInterval = 2.0f + (rand() % 31) / 10.0f; // 2.0 to 5.0 seconds
    if (timerMove >= changeInterval) {
        timerMove = 0.0f;

        int gridX = (x + 10) / 30;
        int gridY = (y + 10) / 30;
        std::vector<std::pair<int, int>> directions;

        if (gridX >= 0 && gridX < (int)grid[0].size() && gridY >= 0 && gridY < (int)grid.size()) {
            if (gridY - 1 >= 0 && grid[gridY - 1][gridX] == 1) {
                directions.emplace_back(0, -1); // Up
            }
            if (gridY + 1 < (int)grid.size() && grid[gridY + 1][gridX] == 1) {
                directions.emplace_back(0, 1); // Down
            }
            if (gridX - 1 >= 0 && grid[gridY][gridX - 1] == 1) {
                directions.emplace_back(-1, 0); // Left
            }
            if (gridX + 1 < (int)grid[gridY].size() && grid[gridY][gridX + 1] == 1) {
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

    if (newGridY >= 0 && newGridY < (int)grid.size() && newGridX >= 0 && newGridX < (int)grid[newGridY].size() &&
        grid[newGridY][newGridX] == 1 && newX >= 0 && newX <= windowWidth - 20 && newY >= 0 &&
        newY <= windowHeight - 20) {
        x = newX;
        y = newY;
    } else {
        // bounce back via a new random direction immediately
        std::vector<std::pair<int, int>> validDirs;
        int gridX = (x + 10) / 30;
        int gridY = (y + 10) / 30;

        if (gridY > 0 && grid[gridY - 1][gridX] == 1) validDirs.emplace_back(0, -1);
        if (gridY < (int)grid.size() - 1 && grid[gridY + 1][gridX] == 1) validDirs.emplace_back(0, 1);
        if (gridX > 0 && grid[gridY][gridX - 1] == 1) validDirs.emplace_back(-1, 0);
        if (gridX < (int)grid[gridY].size() - 1 && grid[gridY][gridX + 1] == 1) validDirs.emplace_back(1, 0);

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

bool Enemy::checkRight(const std::vector<std::vector<int>>& grid) const {
    int gridX = (x + 10) / 30;
    int gridY = (y + 10) / 30;
    if (gridX < (int)grid[gridY].size() - 1 && grid[gridY][gridX + 1] == 0) {
        return true;
    }
    return false;
}

bool Enemy::checkLeft(const std::vector<std::vector<int>>& grid) const {
    int gridX = (x + 10) / 30;
    int gridY = (y + 10) / 30;
    if (gridX > 0 && grid[gridY][gridX - 1] == 0) {
        return true;
    }
    return false;
}

bool Enemy::checkUp(const std::vector<std::vector<int>>& grid) const {
    int gridX = (x + 10) / 30;
    int gridY = (y + 10) / 30;
    if (gridY > 0 && grid[gridY - 1][gridX] == 0) {
        return true;
    }
    return false;
}

bool Enemy::checkDown(const std::vector<std::vector<int>>& grid) const {
    int gridX = (x + 10) / 30;
    int gridY = (y + 10) / 30;
    if (gridY < (int)grid.size() - 1 && grid[gridY + 1][gridX] == 0) {
        return true;
    }
    return false;
}

void Friend::update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, float deltaTime) {
    timer += deltaTime;
    // Random change interval between 2 and 5 seconds
    float changeInterval = 2.0f + (rand() % 31) / 10.0f; // 2.0 to 5.0 seconds
    if (timer >= changeInterval) {
        timer = 0.0f;

        int gridX = (x + 10) / 30;
        int gridY = (y + 10) / 30;
        std::vector<std::pair<int, int>> directions;

        if (gridX >= 0 && gridX < (int)grid[0].size() && gridY >= 0 && gridY < (int)grid.size()) {
            if (gridY - 2 >= 0) {
                directions.emplace_back(0, -1); // Up
            }
            if (gridY + 2 < (int)grid.size()) {
                directions.emplace_back(0, 1); // Down
            }
            if (gridX - 2 >= 0) {
                directions.emplace_back(-1, 0); // Left
            }
            if (gridX + 2 < (int)grid[gridY].size()) {
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

    if (newGridY >= 0 && newGridY < (int)grid.size() && newGridX >= 0 && newGridX < (int)grid[newGridY].size() &&
        newX >= 0 && newX <= windowWidth - 20 && newY >= 0 && newY <= windowHeight - 20) {
        x = newX;
        y = newY;
    } else {
        // bounce back via a new random direction immediately
        std::vector<std::pair<int, int>> validDirs;
        int gridX = (x + 10) / 30;
        int gridY = (y + 10) / 30;

        if (gridY > 0) validDirs.emplace_back(0, -1);
        if (gridY < (int)grid.size() - 1) validDirs.emplace_back(0, 1);
        if (gridX > 0) validDirs.emplace_back(-1, 0);
        if (gridX < (int)grid[gridY].size() - 1) validDirs.emplace_back(1, 0);

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

void Trash::update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, float deltaTime) {
    timer += deltaTime;
    // Random change interval between 2 and 5 seconds
    float changeInterval = 2.0f + (rand() % 31) / 10.0f; // 2.0 to 5.0 seconds
    if (timer >= changeInterval) {
        timer = 0.0f;

        int gridX = (x + 10) / 30;
        int gridY = (y + 10) / 30;
        std::vector<std::pair<int, int>> directions;

        if (gridX >= 0 && gridX < (int)grid[0].size() && gridY >= 0 && gridY < (int)grid.size()) {
            if (gridY - 2 >= 0 && grid[gridY - 2][gridX] == 0) {
                directions.emplace_back(0, -1); // Up
            }
            if (gridY + 2 < (int)grid.size() && grid[gridY + 2][gridX] == 0) {
                directions.emplace_back(0, 1); // Down
            }
            if (gridX - 2 >= 0 && grid[gridY][gridX - 2] == 0) {
                directions.emplace_back(-1, 0); // Left
            }
            if (gridX + 2 < (int)grid[gridY].size() && grid[gridY][gridX + 2] == 0) {
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

    if (newGridY >= 0 && newGridY < (int)grid.size() && newGridX >= 0 && newGridX < (int)grid[newGridY].size() &&
        grid[newGridY][newGridX] == 0 && newX >= 0 && newX <= windowWidth - 20 && newY >= 0 &&
        newY <= windowHeight - 20) {
        x = newX;
        y = newY;
    } else {
        // bounce back via a new random direction immediately
        std::vector<std::pair<int, int>> validDirs;
        int gridX = (x + 10) / 30;
        int gridY = (y + 10) / 30;

        // Validate grid indices before accessing
        if (gridX >= 0 && gridX < (int)grid[0].size() && gridY >= 0 && gridY < (int)grid.size()) {
            if (gridY > 0 && grid[gridY - 1][gridX] == 0) validDirs.emplace_back(0, -1);
            if (gridY < (int)grid.size() - 1 && grid[gridY + 1][gridX] == 0) validDirs.emplace_back(0, 1);
            if (gridX > 0 && grid[gridY][gridX - 1] == 0) validDirs.emplace_back(-1, 0);
            if (gridX < (int)grid[gridY].size() - 1 && grid[gridY][gridX + 1] == 0) validDirs.emplace_back(1, 0);
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

void Boat::update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, float deltaTime) {
    float movement = speed * deltaTime;
    int gridX = (int)((x + 10) / 30);
    int gridY = (int)((y + 10) / 30);

    if (moveUp && y - movement >= 0) {
        int nextY = (int)(((y - movement) + 10) / 30);
        if (nextY >= 0 && nextY < (int)grid.size() && gridX >= 0 && gridX < (int)grid[nextY].size() &&
            grid[nextY][gridX] == 0) {
            y -= movement;
        }
    }

    if (moveDown && y + movement < windowHeight) {
        int nextY = (int)(((y + movement) + 10) / 30);
        if (nextY >= 0 && nextY < (int)grid.size() && gridX >= 0 && gridX < (int)grid[nextY].size() &&
            grid[nextY][gridX] == 0) {
            y += movement;
        }
    }
    if (moveLeft && x - movement >= 0) {
        int nextX = (int)(((x - movement) + 10) / 30);
        if (gridY >= 0 && gridY < (int)grid.size() && nextX >= 0 && nextX < (int)grid[gridY].size() &&
            grid[gridY][nextX] == 0) {
            x -= movement;
        }
    }
    if (moveRight && x + movement < windowWidth) {
        int nextX = (int)(((x + movement) + 10) / 30);
        if (gridY >= 0 && gridY < (int)grid.size() && nextX >= 0 && nextX < (int)grid[gridY].size() &&
            grid[gridY][nextX] == 0) {
            x += movement;
        }
    }
}

void Boat::render(SDL_Renderer* ren) const {
    SDL_SetRenderDrawColor(ren, 150, 75, 0, 255); // Brown for boat
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 25, 25};
    SDL_RenderFillRect(ren, &rect);
}

bool Boat::check_up(const std::vector<std::vector<int>>& grid, int i, int j) const {
    if (i > 0 && grid[i][j] == 1) {
        return true;
    }
    return false;
}

bool Boat::check_down(const std::vector<std::vector<int>>& grid, int i, int j) const {
    if (i < grid.size() - 1 && grid[i][j] == 1) {
        return true;
    }
    return false;
}

bool Boat::check_left(const std::vector<std::vector<int>>& grid, int i, int j) const {
    if (j > 0 && grid[i][j] == 1) {
        return true;
    }
    return false;
}

bool Boat::check_right(const std::vector<std::vector<int>>& grid, int i, int j) const {
    if (j < grid[i].size() - 1 && grid[i][j] == 1) {
        return true;
    }
    return false;
}