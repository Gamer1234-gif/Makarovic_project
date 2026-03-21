#pragma once
#include <SDL2/SDL.h>
#include <vector>

class Character {
public:
    virtual void update(int windowWidth, int windowHeight) = 0;
    virtual void render(SDL_Renderer* ren) = 0;

    virtual ~Character() {
    }

    float x, y;
};

class Player : public Character {
public:
    void handleInput(const SDL_Event& e);
    void update(int windowWidth, int windowHeight) override;
    void render(SDL_Renderer* ren) override;
    void Boundaries(int windowWidth, int windowHeight) {
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > windowWidth - 20) x = windowWidth - 20;
        if (y > windowHeight - 20) y = windowHeight - 20;
    };

private:
    float speed = 5.0f;
    bool up = false, down = false, left = false, right = false;
    int maxHeight;
    int maxWidth;
};

class Enemy : public Character {
public:
    void update(int windowWidth, int windowHeight) override;
    void render(SDL_Renderer* ren) override;
};
