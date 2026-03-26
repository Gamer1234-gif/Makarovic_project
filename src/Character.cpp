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

void Player::update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, const Boat& boat) {
    // Calculate boat's grid position
    int boatGridX = (boat.getX() + 10) / 30;
    int boatGridY = (boat.getY() + 10) / 30;

    if (moveUp && y - speed >= 0) {
        int centerX = x + 10;
        int nextY = (y - speed + 10) / 30;
        int gridX = centerX / 30;

        if (grid[nextY][gridX] == 1 || (nextY == boatGridY && gridX == boatGridX)) {
            y -= speed;
        }
    }
    if (moveDown && y + speed <= windowHeight - 20) {
        int centerX = x + 10;
        int nextY = (y + speed + 10) / 30;
        int gridX = centerX / 30;

        if (grid[nextY][gridX] == 1 || (nextY == boatGridY && gridX == boatGridX)) {
            y += speed;
        }
    }
    if (moveLeft && x - speed >= 0) {
        int centerY = y + 10;
        int nextX = (x - speed + 10) / 30;
        int gridY = centerY / 30;

        if (grid[gridY][nextX] == 1 || (gridY == boatGridY && nextX == boatGridX)) {
            x -= speed;
        }
    }
    if (moveRight && x + speed <= windowWidth - 20) {
        int centerY = y + 10;
        int nextX = (x + speed + 10) / 30;
        int gridY = centerY / 30;

        if (grid[gridY][nextX] == 1 || (gridY == boatGridY && nextX == boatGridX)) {
            x += speed;
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

void Enemy::update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, float deltaTime) {
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

void Friend::update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, float deltaTime) {
    // Simple AI: move towards player or something, but for now, static
    // std::cout << "Friend position: (" << x << ", " << y << ")\n";
}

void Friend::render(SDL_Renderer* ren) const {
    SDL_SetRenderDrawColor(ren, 20, 255, 125, 255); // Green for friend
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 20, 20};
    SDL_RenderFillRect(ren, &rect);
}

void Trash::update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid, float deltaTime) {
    timer += deltaTime;
    if (timer >= 2.0f) {
        timer = 0.0f;

        int gridX = (x + 10) / 30;
        int gridY = (y + 10) / 30;
        std::vector<std::pair<int, int>> directions;

        if (gridY - 2 >= 0 && grid[gridY - 2][gridX] == 0) {
            directions.emplace_back(0, -1); // Up
        }
        if (gridY + 2 <= (int)grid.size() - 1 && grid[gridY + 2][gridX] == 0) {
            directions.emplace_back(0, 1); // Down
        }
        if (gridX - 2 >= 0 && grid[gridY][gridX - 2] == 0) {
            directions.emplace_back(-1, 0); // Left
        }
        if (gridX + 2 <= (int)grid[gridY].size() - 1 && grid[gridY][gridX + 2] == 0) {
            directions.emplace_back(1, 0); // Right
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

        if (gridY > 0 && grid[gridY - 1][gridX] == 0) validDirs.emplace_back(0, -1);
        if (gridY < (int)grid.size() - 1 && grid[gridY + 1][gridX] == 0) validDirs.emplace_back(0, 1);
        if (gridX > 0 && grid[gridY][gridX - 1] == 0) validDirs.emplace_back(-1, 0);
        if (gridX < (int)grid[gridY].size() - 1 && grid[gridY][gridX + 1] == 0) validDirs.emplace_back(1, 0);

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

void Boat::update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid) {
    int gridX = (x + 10) / 30;
    int gridY = (y + 10) / 30;

    if (moveUp && y - speed >= 0) {
        if (grid[(y - speed + 10) / 30][gridX] == 0) {
            y -= speed;
        }
    }

    if (moveDown && y + speed <= windowHeight - 20) {
        if (grid[(y + speed + 10) / 30][gridX] == 0) {
            y += speed;
        }
    }
    if (moveLeft && x - speed >= 0) {
        if (grid[gridY][(x - speed + 10) / 30] == 0) {
            x -= speed;
        }
    }
    if (moveRight && x + speed <= windowWidth - 20) {
        if (grid[gridY][(x + speed + 10) / 30] == 0) {
            x += speed;
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