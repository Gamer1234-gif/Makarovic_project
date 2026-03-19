#include "Character.h"

void Player::handleInput(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_a: x -= speed; break;
        case SDLK_d: x += speed; break;
        case SDLK_w: y -= speed; break;
        case SDLK_s: y += speed; break;
        }
    }
}

void Player::update() {
    // Additional update logic if needed
}

void Player::render(SDL_Renderer* ren) {
    SDL_SetRenderDrawColor(ren, 255, 255, 0, 255); // Yellow for player
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 20, 20};
    SDL_RenderFillRect(ren, &rect);
}

void Enemy::update() {
    // Simple AI: move towards player or something, but for now, static
}

void Enemy::render(SDL_Renderer* ren) {
    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); // Red for enemy
    SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), 20, 20};
    SDL_RenderFillRect(ren, &rect);
}