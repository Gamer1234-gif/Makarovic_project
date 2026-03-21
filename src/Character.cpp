#include "Character.h"
#include <iostream>

void Player::handleInput(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_w: up = true; break;
        case SDLK_s: down = true; break;
        case SDLK_a: left = true; break;
        case SDLK_d: right = true; break;
        }
    }

    if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
        case SDLK_w: up = false; break;
        case SDLK_s: down = false; break;
        case SDLK_a: left = false; break;
        case SDLK_d: right = false; break;
        }
    }
}

void Player::update(int windowWidth, int windowHeight) {
    if (up && y - speed >= 0) y -= speed;
    if (down && y + speed <= windowHeight - 20) y += speed;
    if (left && x - speed >= 0) x -= speed;
    if (right && x + speed <= windowWidth - 20) x += speed;
    // std::cout << "Player position: (" << x << ", " << y << ")\n";
}

void Player::render(SDL_Renderer* ren) const {
    SDL_SetRenderDrawColor(ren, 255, 255, 0, 255); // Yellow for player
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 20, 20};
    SDL_RenderFillRect(ren, &rect);
}

void Enemy::update(int windowWidth, int windowHeight) {
    // Simple AI: move towards player or something, but for now, static
    // std::cout << "Enemy position: (" << x << ", " << y << ")\n";
}

void Enemy::render(SDL_Renderer* ren) const {
    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); // Red for enemy
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 20, 20};
    SDL_RenderFillRect(ren, &rect);
}