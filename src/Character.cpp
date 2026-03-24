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

void Player::boatExitInput(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LSHIFT) {
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
}

void Enemy::update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid) {
    // Simple AI: move towards player or something, but for now, static
    // std::cout << "Enemy position: (" << x << ", " << y << ")\n";
}

void Enemy::render(SDL_Renderer* ren) const {
    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); // Red for enemy
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 20, 20};
    SDL_RenderFillRect(ren, &rect);
}

void Friend::update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid) {
    // Simple AI: move towards player or something, but for now, static
    // std::cout << "Friend position: (" << x << ", " << y << ")\n";
}

void Friend::render(SDL_Renderer* ren) const {
    SDL_SetRenderDrawColor(ren, 20, 255, 125, 255); // Green for friend
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 20, 20};
    SDL_RenderFillRect(ren, &rect);
}

void Trash::update(int windowWidth, int windowHeight, const std::vector<std::vector<int>>& grid) {
    // Static, no update needed
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
    if (moveUp && y - speed >= 0) y -= speed;
    if (moveDown && y + speed <= windowHeight - 20) y += speed;
    if (moveLeft && x - speed >= 0) x -= speed;
    if (moveRight && x + speed <= windowWidth - 20) x += speed;
}

void Boat::render(SDL_Renderer* ren) const {
    SDL_SetRenderDrawColor(ren, 150, 75, 0, 255); // Brown for boat
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 25, 25};
    SDL_RenderFillRect(ren, &rect);
}

bool Boat::check_up(const std::vector<std::vector<int>>& grid, int i, int j) {
    if (i > 0 && grid[i][j] == 1) {
        return true;
    }
    return false;
}

bool Boat::check_down(const std::vector<std::vector<int>>& grid, int i, int j) {
    if (i < grid.size() - 1 && grid[i][j] == 1) {
        return true;
    }
    return false;
}

bool Boat::check_left(const std::vector<std::vector<int>>& grid, int i, int j) {
    if (j > 0 && grid[i][j] == 1) {
        return true;
    }
    return false;
}

bool Boat::check_right(const std::vector<std::vector<int>>& grid, int i, int j) {
    if (j < grid[i].size() - 1 && grid[i][j] == 1) {
        return true;
    }
    return false;
}