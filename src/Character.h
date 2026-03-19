#pragma once
#include <SDL2/SDL.h>
#include <vector>

class Character {
public:
    virtual void update() = 0;
    virtual void render(SDL_Renderer* ren) = 0;

    virtual ~Character() {
    }

    float x, y;
};

class Player : public Character {
public:
    void handleInput(const SDL_Event& e);
    void update() override;
    void render(SDL_Renderer* ren) override;

private:
    float speed = 5.0f;
};

class Enemy : public Character {
public:
    void update() override;
    void render(SDL_Renderer* ren) override;
};
