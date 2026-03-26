#include "Character.h"
#include "Game.h"
#include <SDL2/SDL_ttf.h>
#include <iostream>


bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return false;
    }

    win = SDL_CreateWindow(
        "GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN
    );

    if (!win) return false;

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!ren) return false;

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << "\n";
        return false;
    }

    font = TTF_OpenFont("arial.ttf", 14);
    if (!font) {
        std::cerr << "TTF_OpenFont failed: " << TTF_GetError() << "\n";
        return false;
    }

    srand(time(NULL)); // Seed random number generator

    // GRID GENERATION
    grid.resize(GRID_SIZE, std::vector<int>(GRID_SIZE));

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            double n = noise.noise(i * frequency, j * frequency);
            n = (n + 1.0) / 2.0;
            grid[i][j] = (n > threshold) ? 1 : 0;
        }
    }

    player.Spawn(grid, SQUARE_SIZE, windowWidth, windowHeight);
    boat.Spawn(grid, SQUARE_SIZE, windowWidth, windowHeight);

    // Initialize enemy in the middle of the top left quadrant
    for (int i = 0; i < 10; ++i) {
        Enemy e;
        e.Spawn(grid, SQUARE_SIZE, windowWidth, windowHeight);
        enemies.push_back(e);
        enemiesRemaining++;
    }

    for (int i = 0; i < GRID_SIZE / 2; ++i) {
        Trash t;
        t.Spawn(grid, SQUARE_SIZE, windowWidth, windowHeight);
        trash.push_back(t);
        trashRemaining++;
    }

    for (int i = 0; i < 10; ++i) {
        Friend f;
        f.Spawn(grid, SQUARE_SIZE, windowWidth, windowHeight);
        friends.push_back(f);
        friendsRemaining++;
    }


    running = true;
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_e) {
            SDL_Rect boatRect = boat.getRect();
            SDL_Rect pRect = player.getRect();
            if (SDL_HasIntersection(&pRect, &boatRect)) {
                player.setInBoat(true);
                boat.setplayerInBoat(true);
            }
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LSHIFT) {
            player.boatExitInput(event, grid, boat);
            boat.setplayerInBoat(false);
        }
        if (player.getInBoat()) {
            boat.handleInput(event);
        } else {
            player.handleInput(event);
        }
    }
}

void Game::update() {
    player.update(windowWidth, windowHeight, grid, boat);
    player.Boundaries(windowWidth, windowHeight);
    for (auto& enemy : enemies) {
        enemy.update(windowWidth, windowHeight, grid, deltaTime);
    }

    for (auto it = enemies.begin(); it != enemies.end();) {
        SDL_Rect pRect = player.getRect();
        SDL_Rect eRect = it->getRect();

        if (SDL_HasIntersection(&pRect, &eRect) && !it->nearbyEnemy(enemies)) {
            std::cout << "Enemy destroyed!\n";
            it = enemies.erase(it);
            score++;
            enemiesRemaining--;
        } else {
            if (SDL_HasIntersection(&pRect, &eRect) && it->nearbyEnemy(enemies)) {
                std::cout << "Player hit by enemy!\n";
                player.Spawn(grid, SQUARE_SIZE, windowWidth, windowHeight);
            }
            ++it;
        }
    }

    for (auto it = friends.begin(); it != friends.end();) {
        SDL_Rect pRect = player.getRect();
        SDL_Rect fRect = it->getRect();

        if (SDL_HasIntersection(&pRect, &fRect)) {
            std::cout << "Wrong one!\n";
            it = friends.erase(it);
            score -= 2; // Penalty for hitting a friend
            friendsRemaining--;
        } else {
            ++it;
        }
    }

    for (auto it = trash.begin(); it != trash.end();) {
        SDL_Rect pRect = player.getRect();
        SDL_Rect tRect = it->getRect();

        if (SDL_HasIntersection(&pRect, &tRect)) {
            std::cout << "Trash collected!\n";
            it = trash.erase(it);
            score++;
            trashRemaining--;
        } else {
            ++it;
        }
    }

    if (player.getInBoat()) {
        boat.update(windowWidth, windowHeight, grid);
        player.setX(boat.getX());
        player.setY(boat.getY());
    }

    for (auto& trashItem : trash) {
        trashItem.update(windowWidth, windowHeight, grid, deltaTime);
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid[i][j] == 1) {
                SDL_SetRenderDrawColor(ren, 0, 200, 0, 255); // Green for land
            } else {
                SDL_SetRenderDrawColor(ren, 0, 0, 200, 255); // Blue for water
            }
            SDL_Rect cell{j * SQUARE_SIZE, i * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
            SDL_RenderFillRect(ren, &cell);
        }
    }

    player.render(ren);
    boat.render(ren);
    for (const auto& enemy : enemies) {
        enemy.render(ren);
    }

    for (const auto& friend_ : friends) {
        friend_.render(ren);
    }

    for (const auto& t : trash) {
        t.render(ren);
    }

    if (font) {
        std::string text = "Score: " + std::to_string(score) + " Enemies: " + std::to_string(enemiesRemaining) +
                           " Trash: " + std::to_string(trashRemaining) +
                           " Friends: " + std::to_string(friendsRemaining);

        SDL_Color color = {255, 255, 255, 255}; // white
        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
            if (texture) {
                SDL_Rect dstRect = {20, 20, surface->w, surface->h};
                SDL_RenderCopy(ren, texture, NULL, &dstRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }

    SDL_RenderPresent(ren);
    for (const auto& enemy : enemies) {
        enemy.render(ren);
    }

    for (const auto& friend_ : friends) {
        friend_.render(ren);
    }

    for (const auto& t : trash) {
        t.render(ren);
    }

    SDL_RenderPresent(ren);
}

void Game::run() {
    while (running) {
        setdeltaTime((SDL_GetTicks() - getlastTime()) / 1000.0f);
        setlastTime(SDL_GetTicks());

        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}

void Game::clean() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    TTF_Quit();

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}