#include "Character.h"
#include "Game.h"
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <cstdio>
#include <iostream>

void Game::loadLevel(int levelIndex) {
    if (levelIndex < 0 || levelIndex >= static_cast<int>(levels.size())) {
        std::cerr << "Invalid level index: " << levelIndex << "\n";
        return;
    }

    const Level& level = levels[levelIndex];
    noise = PerlinNoise(level.seed);

    grid.clear();
    grid.resize(GRID_SIZE, std::vector<int>(GRID_SIZE));

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            double n = noise.noise(i * frequency, j * frequency);
            n = (n + 1.0) / 2.0;
            grid[i][j] = (n > threshold) ? 1 : 0;
        }
    }

    player.Spawn(grid, SQUARE_SIZE, windowWidth, windowHeight);
    player.setSpeed(level.playerSpeed);

    // Store original position for respawning
    originalPlayerX = player.getX();
    originalPlayerY = player.getY();
    originalBoatX = boat.getX();
    originalBoatY = boat.getY();

    if (player.getInBoat()) {
        boat.SpawnInOcean(player.getX(), player.getY());
    } else {
        boat.Spawn(grid, SQUARE_SIZE, windowWidth, windowHeight);
    }
    boat.setSpeed(level.playerSpeed);

    enemies.clear();
    for (int i = 0; i < level.enemyCount; ++i) {
        Enemy e;
        e.Spawn(grid, SQUARE_SIZE, windowWidth, windowHeight);
        e.setSpeed(level.enemySpeed);
        e.giveTrash();
        enemies.push_back(e);
    }
    enemiesRemaining = level.enemyCount;

    trash.clear();
    for (int i = 0; i < level.trashCount; ++i) {
        Trash t;
        t.Spawn(grid, SQUARE_SIZE, windowWidth, windowHeight);
        t.setSpeed(level.trashSpeed);
        trash.push_back(t);
    }
    trashRemaining = level.trashCount;

    friends.clear();
    for (int i = 0; i < level.friendCount; ++i) {
        Friend f;
        f.Spawn(grid, SQUARE_SIZE, windowWidth, windowHeight);
        f.setSpeed(level.friendSpeed);
        friends.push_back(f);
    }
    friendsRemaining = level.friendCount;

    score = 0;
}

bool Game::init() {
    levels.push_back(
        {10, 40, 10, 80.0f, 80.0f, 85.0f, 85.0f, 16305}
    ); // Level 1: 10 enemies, 40 trash, 10 friends, enemy speed 80, trash speed 80, player speed 85, friend speed
       // 85, seed 16305
    levels.push_back(
        {20, 60, 5, 200.0f, 200.0f, 50.0f, 50.0f, 18320}
    ); // Level 2: 20 enemies, 60 trash, 5 friends, enemy speed 200, trash speed 200, player speed 50, friend speed
       // 50, seed 18320

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return false;
    }

    win = SDL_CreateWindow(
        "GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
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

    largeFont = TTF_OpenFont("arial.ttf", 48);
    if (!largeFont) {
        std::cerr << "TTF_OpenFont for large font failed: " << TTF_GetError() << "\n";
        return false;
    }

    srand(time(NULL)); // Seed random number generator

    gameState = MAIN_MENU;
    running = true;
    return true;
}

void Game::handleEvents() {
    switch (gameState) {
    case MAIN_MENU: handleMenuEvents(); break;
    case NAME_INPUT: handleNameInputEvents(); break;
    case LEVEL_SELECT: handleLevelSelectEvents(); break;
    case PLAYING:
    case DEATH_SCREEN: {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                if (gameState == PLAYING) {
                    saveGameState();
                    running = false;
                } else {
                    running = false;
                }
            }
            if (gameState == DEATH_SCREEN && event.type == SDL_KEYDOWN && inputLockTimer <= 0.0f) {
                if (resumedGameIsDead) {
                    // Only delete save if this was a resumed game that died
                    deleteSavedGame(playerName);
                    resumedGameIsDead = false;
                    isResumedGame = false;
                }
                gameState = MAIN_MENU;
                playerName = "";
            }
            if (gameState == PLAYING) {
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
        break;
    }
    case GAME_OVER: break;
    }
}

void Game::handleMenuEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
            if (event.key.keysym.sym == SDLK_RETURN) {
                gameState = NAME_INPUT;
            }
        }
    }
}

void Game::handleNameInputEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                gameState = MAIN_MENU;
                playerName = "";
            }
            if (event.key.keysym.sym == SDLK_RETURN && !playerName.empty()) {
                // Check if player is blacklisted (died before)
                if (isBlacklisted(playerName)) {
                    // Player has died before - cannot play
                    gameState = MAIN_MENU;
                    playerName = "";
                    std::cout << "This player has already died and cannot continue.\n";
                } else if (hasSavedGame(playerName)) {
                    SavedGame savedGame = loadGameState(playerName);
                    isResumedGame = true;
                    resumedGameIsDead = savedGame.isDead;

                    if (resumedGameIsDead) {
                        // If player died before, show death screen
                        gameState = DEATH_SCREEN;
                        deathScreenTimer = DEATH_SCREEN_DURATION;
                        inputLockTimer = INPUT_LOCK_DURATION;
                    } else {
                        // Resume the game
                        currentLevel = savedGame.level - 1;
                        score = savedGame.score;
                        loadLevel(currentLevel);
                        // Restore player position
                        player.setX(savedGame.playerX);
                        player.setY(savedGame.playerY);
                        boat.setX(savedGame.boatX);
                        boat.setY(savedGame.boatY);
                        // Restore boat state
                        player.setInBoat(savedGame.inBoat);
                        gameState = PLAYING;
                    }
                } else {
                    // New player, go to level select
                    gameState = LEVEL_SELECT;
                }
            }
            if (event.key.keysym.sym == SDLK_BACKSPACE && !playerName.empty()) {
                playerName.pop_back();
            }
        }
        if (event.type == SDL_TEXTINPUT) {
            if (playerName.length() < 20) {
                playerName += event.text.text;
            }
        }
    }
}

void Game::handleLevelSelectEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                gameState = NAME_INPUT;
                playerName = "";
            }
            if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_UP) {
                selectedLevel = 0;
            }
            if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_DOWN) {
                selectedLevel = 1;
            }
            if (event.key.keysym.sym == SDLK_RETURN) {
                currentLevel = selectedLevel;
                loadLevel(currentLevel);
                gameState = PLAYING;
            }
        }
    }
}

void Game::update() {
    if (gameState != PLAYING) {
        if (gameState == DEATH_SCREEN) {
            deathScreenTimer -= deltaTime;
            inputLockTimer -= deltaTime;
            if (deathScreenTimer <= 0.0f) {
                // Auto transition after timeout (optional, user can also press SPACE)
                // resetPlayerPosition();
                // gameState = PLAYING;
            }
        }
        return;
    }

    player.update(windowWidth, windowHeight, grid, boat, deltaTime);
    player.Boundaries(windowWidth, windowHeight);
    for (auto& friend_ : friends) {
        friend_.update(windowWidth, windowHeight, grid, deltaTime);
    }

    for (auto& enemy : enemies) {
        enemy.Enemy_timer(deltaTime);
        enemy.updateTrashDropTimer(deltaTime);
        Trash t;
        enemy.update(windowWidth, windowHeight, grid, deltaTime);
        if (enemy.checkRight(grid) && enemy.getHasTrash() && enemy.canDropTrash()) {
            t.SpawnFromEnemy(enemy, grid, windowWidth, windowHeight);
            t.setSpeed(levels[currentLevel].trashSpeed);
            trash.push_back(t);
            trashRemaining++;
            enemy.setHasTrash(false);
            enemy.resetTrashDropTimer();
        }

        if (enemy.checkLeft(grid) && enemy.getHasTrash() && enemy.canDropTrash()) {
            t.SpawnFromEnemy(enemy, grid, windowWidth, windowHeight);
            t.setSpeed(levels[currentLevel].trashSpeed);
            trash.push_back(t);
            trashRemaining++;
            enemy.setHasTrash(false);
            enemy.resetTrashDropTimer();
        }

        if (enemy.checkUp(grid) && enemy.getHasTrash() && enemy.canDropTrash()) {
            t.SpawnFromEnemy(enemy, grid, windowWidth, windowHeight);
            t.setSpeed(levels[currentLevel].trashSpeed);
            trash.push_back(t);
            trashRemaining++;
            enemy.setHasTrash(false);
            enemy.resetTrashDropTimer();
        }

        if (enemy.checkDown(grid) && enemy.getHasTrash() && enemy.canDropTrash()) {
            t.SpawnFromEnemy(enemy, grid, windowWidth, windowHeight);
            t.setSpeed(levels[currentLevel].trashSpeed);
            trash.push_back(t);
            trashRemaining++;
            enemy.setHasTrash(false);
            enemy.resetTrashDropTimer();
        }
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
                // Save death state to game_saves.txt
                std::ofstream deadFile("game_saves.txt", std::ios::app);
                if (deadFile.is_open()) {
                    deadFile << playerName << " | Score: " << score << " | Level: " << (currentLevel + 1)
                             << " | Dead: 1\n";
                    deadFile.close();
                }
                // Add to blacklist so they can't play again
                addToBlacklist(playerName);
                // Also save to results
                saveGameResult();
                gameState = DEATH_SCREEN;
                deathScreenTimer = DEATH_SCREEN_DURATION;
                return;
            }
            ++it;
        }
    }

    for (auto it = friends.begin(); it != friends.end();) {
        SDL_Rect fRect = it->getRect();
        for (auto it_enemy = enemies.begin(); it_enemy != enemies.end();) {
            SDL_Rect eRect = it_enemy->getRect();
            if (SDL_HasIntersection(&fRect, &eRect) &&
                (it->nearbyFriend(friends) + it->nearbyPlayer(player) >= it_enemy->nearbyEnemy(enemies))) {
                std::cout << "Friend hit enemy!\n";
                it_enemy = enemies.erase(it_enemy);
                score++;
                enemiesRemaining--;
                break;
            } else {
                if (SDL_HasIntersection(&fRect, &eRect)) {
                    std::cout << "Friend hit by enemy!\n";
                    it = friends.erase(it);
                    friendsRemaining--;
                    break;
                }
            }
            it_enemy++;
        }
        it++;
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

    for (auto it = friends.begin(); it != friends.end();) {
        SDL_Rect fRect = it->getRect();
        for (auto it_enemy = enemies.begin(); it_enemy != enemies.end();) {
            SDL_Rect eRect = it_enemy->getRect();
            if (SDL_HasIntersection(&fRect, &eRect)) {
                std::cout << "Friend hit enemy!\n";
                it_enemy = enemies.erase(it_enemy);
                score++;
                enemiesRemaining--;
                break;
            }
            ++it_enemy;
        }
        ++it;
    }

    for (auto it = friends.begin(); it != friends.end();) {
        SDL_Rect fRect = it->getRect();
        for (auto it_trash = trash.begin(); it_trash != trash.end();) {
            SDL_Rect tRect = it_trash->getRect();
            if (SDL_HasIntersection(&fRect, &tRect)) {
                std::cout << "Friend hit trash!\n";
                it_trash = trash.erase(it_trash);
                score++;
                trashRemaining--;
                break;
            }
            ++it_trash;
        }
        ++it;
    }

    if (player.getInBoat()) {
        boat.update(windowWidth, windowHeight, grid, deltaTime);
        player.setX(boat.getX());
        player.setY(boat.getY());
    }

    for (auto& trashItem : trash) {
        trashItem.update(windowWidth, windowHeight, grid, deltaTime);
    }

    if (enemiesRemaining == 0 && trashRemaining == 0) {
        currentLevel++;
        if (currentLevel < static_cast<int>(levels.size())) {
            loadLevel(currentLevel);
        } else {
            std::cout << "Congratulations! You've completed all levels!\n";
            saveGameResult();
            running = false;
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    switch (gameState) {
    case MAIN_MENU: renderMainMenu(); break;
    case NAME_INPUT: renderNameInput(); break;
    case LEVEL_SELECT: renderLevelSelect(); break;
    case PLAYING:
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
            std::string text = "Player: " + playerName + " | Score: " + std::to_string(score) +
                               " | Enemies: " + std::to_string(enemiesRemaining) +
                               " | Trash: " + std::to_string(trashRemaining) +
                               " | Friends: " + std::to_string(friendsRemaining);

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
        break;
    case DEATH_SCREEN:
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                if (grid[i][j] == 1) {
                    SDL_SetRenderDrawColor(ren, 0, 200, 0, 255);
                } else {
                    SDL_SetRenderDrawColor(ren, 0, 0, 200, 255);
                }
                SDL_Rect cell{j * SQUARE_SIZE, i * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
                SDL_RenderFillRect(ren, &cell);
            }
        }
        renderDeathScreen();
        break;
    case GAME_OVER: break;
    }

    SDL_RenderPresent(ren);
}

void Game::renderMainMenu() {
    SDL_SetRenderDrawColor(ren, 20, 20, 40, 255);
    SDL_RenderClear(ren);

    if (largeFont) {
        SDL_Color titleColor = {100, 200, 255, 255};
        SDL_Surface* titleSurface = TTF_RenderText_Solid(largeFont, "MAIN MENU", titleColor);
        if (titleSurface) {
            SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(ren, titleSurface);
            if (titleTexture) {
                SDL_Rect titleRect = {windowWidth / 2 - titleSurface->w / 2, 100, titleSurface->w, titleSurface->h};
                SDL_RenderCopy(ren, titleTexture, NULL, &titleRect);
                SDL_DestroyTexture(titleTexture);
            }
            SDL_FreeSurface(titleSurface);
        }
    }

    if (font) {
        SDL_Color instructColor = {200, 200, 200, 255};
        SDL_Surface* instructSurface = TTF_RenderText_Solid(font, "Press ENTER to start", instructColor);
        if (instructSurface) {
            SDL_Texture* instructTexture = SDL_CreateTextureFromSurface(ren, instructSurface);
            if (instructTexture) {
                SDL_Rect instructRect = {
                    windowWidth / 2 - instructSurface->w / 2, windowHeight / 2, instructSurface->w, instructSurface->h
                };
                SDL_RenderCopy(ren, instructTexture, NULL, &instructRect);
                SDL_DestroyTexture(instructTexture);
            }
            SDL_FreeSurface(instructSurface);
        }

        SDL_Surface* escapeSurface = TTF_RenderText_Solid(font, "Press ESC to exit", instructColor);
        if (escapeSurface) {
            SDL_Texture* escapeTexture = SDL_CreateTextureFromSurface(ren, escapeSurface);
            if (escapeTexture) {
                SDL_Rect escapeRect = {
                    windowWidth / 2 - escapeSurface->w / 2, windowHeight / 2 + 50, escapeSurface->w, escapeSurface->h
                };
                SDL_RenderCopy(ren, escapeTexture, NULL, &escapeRect);
                SDL_DestroyTexture(escapeTexture);
            }
            SDL_FreeSurface(escapeSurface);
        }
    }

    SDL_RenderPresent(ren);
}

void Game::renderNameInput() {
    SDL_SetRenderDrawColor(ren, 20, 20, 40, 255);
    SDL_RenderClear(ren);

    if (largeFont) {
        SDL_Color titleColor = {100, 200, 255, 255};
        SDL_Surface* titleSurface = TTF_RenderText_Solid(largeFont, "Enter Your Name", titleColor);
        if (titleSurface) {
            SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(ren, titleSurface);
            if (titleTexture) {
                SDL_Rect titleRect = {windowWidth / 2 - titleSurface->w / 2, 100, titleSurface->w, titleSurface->h};
                SDL_RenderCopy(ren, titleTexture, NULL, &titleRect);
                SDL_DestroyTexture(titleTexture);
            }
            SDL_FreeSurface(titleSurface);
        }
    }

    if (font) {
        SDL_Color inputColor = {255, 255, 255, 255};
        std::string displayName = playerName.empty() ? "_" : playerName;
        SDL_Surface* nameSurface = TTF_RenderText_Solid(font, displayName.c_str(), inputColor);
        if (nameSurface) {
            SDL_Texture* nameTexture = SDL_CreateTextureFromSurface(ren, nameSurface);
            if (nameTexture) {
                SDL_Rect nameRect = {
                    windowWidth / 2 - nameSurface->w / 2, windowHeight / 2 - 50, nameSurface->w, nameSurface->h
                };
                SDL_RenderCopy(ren, nameTexture, NULL, &nameRect);
                SDL_DestroyTexture(nameTexture);
            }
            SDL_FreeSurface(nameSurface);
        }

        SDL_Color instructColor = {200, 200, 200, 255};
        SDL_Surface* instructSurface =
            TTF_RenderText_Solid(font, "Type your name and press ENTER (ESC to go back)", instructColor);
        if (instructSurface) {
            SDL_Texture* instructTexture = SDL_CreateTextureFromSurface(ren, instructSurface);
            if (instructTexture) {
                SDL_Rect instructRect = {
                    windowWidth / 2 - instructSurface->w / 2, windowHeight / 2 + 50, instructSurface->w,
                    instructSurface->h
                };
                SDL_RenderCopy(ren, instructTexture, NULL, &instructRect);
                SDL_DestroyTexture(instructTexture);
            }
            SDL_FreeSurface(instructSurface);
        }
    }

    SDL_RenderPresent(ren);
}

void Game::renderLevelSelect() {
    SDL_SetRenderDrawColor(ren, 20, 20, 40, 255);
    SDL_RenderClear(ren);

    if (largeFont) {
        SDL_Color titleColor = {100, 200, 255, 255};
        SDL_Surface* titleSurface = TTF_RenderText_Solid(largeFont, "Select Level", titleColor);
        if (titleSurface) {
            SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(ren, titleSurface);
            if (titleTexture) {
                SDL_Rect titleRect = {windowWidth / 2 - titleSurface->w / 2, 100, titleSurface->w, titleSurface->h};
                SDL_RenderCopy(ren, titleTexture, NULL, &titleRect);
                SDL_DestroyTexture(titleTexture);
            }
            SDL_FreeSurface(titleSurface);
        }
    }

    if (font) {
        // Level 1
        SDL_Color level1Color = (selectedLevel == 0) ? SDL_Color{255, 255, 0, 255} : SDL_Color{200, 200, 200, 255};
        SDL_Surface* level1Surface = TTF_RenderText_Solid(font, "LEVEL 1 (Easy)", level1Color);
        if (level1Surface) {
            SDL_Texture* level1Texture = SDL_CreateTextureFromSurface(ren, level1Surface);
            if (level1Texture) {
                SDL_Rect level1Rect = {
                    windowWidth / 2 - level1Surface->w / 2, windowHeight / 2 - 100, level1Surface->w, level1Surface->h
                };
                SDL_RenderCopy(ren, level1Texture, NULL, &level1Rect);
                SDL_DestroyTexture(level1Texture);
            }
            SDL_FreeSurface(level1Surface);
        }

        // Level 2
        SDL_Color level2Color = (selectedLevel == 1) ? SDL_Color{255, 255, 0, 255} : SDL_Color{200, 200, 200, 255};
        SDL_Surface* level2Surface = TTF_RenderText_Solid(font, "LEVEL 2 (Hard)", level2Color);
        if (level2Surface) {
            SDL_Texture* level2Texture = SDL_CreateTextureFromSurface(ren, level2Surface);
            if (level2Texture) {
                SDL_Rect level2Rect = {
                    windowWidth / 2 - level2Surface->w / 2, windowHeight / 2 + 50, level2Surface->w, level2Surface->h
                };
                SDL_RenderCopy(ren, level2Texture, NULL, &level2Rect);
                SDL_DestroyTexture(level2Texture);
            }
            SDL_FreeSurface(level2Surface);
        }

        SDL_Color instructColor = {200, 200, 200, 255};
        SDL_Surface* instructSurface =
            TTF_RenderText_Solid(font, "Use LEFT/RIGHT or UP/DOWN to select, ENTER to start", instructColor);
        if (instructSurface) {
            SDL_Texture* instructTexture = SDL_CreateTextureFromSurface(ren, instructSurface);
            if (instructTexture) {
                SDL_Rect instructRect = {
                    windowWidth / 2 - instructSurface->w / 2, windowHeight / 2 + 200, instructSurface->w,
                    instructSurface->h
                };
                SDL_RenderCopy(ren, instructTexture, NULL, &instructRect);
                SDL_DestroyTexture(instructTexture);
            }
            SDL_FreeSurface(instructSurface);
        }
    }

    SDL_RenderPresent(ren);
}

void Game::renderDeathScreen() {
    if (largeFont) {
        SDL_Color deathColor = {255, 0, 0, 255};
        SDL_Surface* deathSurface = TTF_RenderText_Solid(largeFont, "YOU DIED!", deathColor);
        if (deathSurface) {
            SDL_Texture* deathTexture = SDL_CreateTextureFromSurface(ren, deathSurface);
            if (deathTexture) {
                SDL_Rect deathRect = {windowWidth / 2 - deathSurface->w / 2, 20, deathSurface->w, deathSurface->h};
                SDL_RenderCopy(ren, deathTexture, NULL, &deathRect);
                SDL_DestroyTexture(deathTexture);
            }
            SDL_FreeSurface(deathSurface);
        }
    }

    // Display top 5 results
    if (font) {
        SDL_Color titleColor = {255, 215, 0, 255};
        SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "TOP 5 RESULTS", titleColor);
        if (titleSurface) {
            SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(ren, titleSurface);
            if (titleTexture) {
                SDL_Rect titleRect = {windowWidth / 2 - titleSurface->w / 2, 100, titleSurface->w, titleSurface->h};
                SDL_RenderCopy(ren, titleTexture, NULL, &titleRect);
                SDL_DestroyTexture(titleTexture);
            }
            SDL_FreeSurface(titleSurface);
        }

        std::vector<GameResult> top5 = getTop5Results();
        SDL_Color scoreColor = {200, 200, 200, 255};
        int yOffset = 140;
        for (size_t i = 0; i < top5.size(); ++i) {
            std::string scoreText =
                std::to_string(i + 1) + ". " + top5[i].playerName + " - Score: " + std::to_string(top5[i].score);
            SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), scoreColor);
            if (scoreSurface) {
                SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(ren, scoreSurface);
                if (scoreTexture) {
                    SDL_Rect scoreRect = {
                        windowWidth / 2 - scoreSurface->w / 2, yOffset, scoreSurface->w, scoreSurface->h
                    };
                    SDL_RenderCopy(ren, scoreTexture, NULL, &scoreRect);
                    SDL_DestroyTexture(scoreTexture);
                }
                SDL_FreeSurface(scoreSurface);
            }
            yOffset += 30;
        }
    }

    if (font) {
        SDL_Color instructColor = {200, 200, 200, 255};
        SDL_Surface* instructSurface = TTF_RenderText_Solid(font, "Press any key to return to menu", instructColor);
        if (instructSurface) {
            SDL_Texture* instructTexture = SDL_CreateTextureFromSurface(ren, instructSurface);
            if (instructTexture) {
                SDL_Rect instructRect = {
                    windowWidth / 2 - instructSurface->w / 2, windowHeight - 50, instructSurface->w, instructSurface->h
                };
                SDL_RenderCopy(ren, instructTexture, NULL, &instructRect);
                SDL_DestroyTexture(instructTexture);
            }
            SDL_FreeSurface(instructSurface);
        }
    }
}

void Game::run() {
    setlastTime(SDL_GetTicks()); // Initialize lastTime on first frame
    SDL_StartTextInput();
    while (running) {
        setdeltaTime((SDL_GetTicks() - getlastTime()) / 1000.0f);
        setlastTime(SDL_GetTicks());

        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
    SDL_StopTextInput();
}

void Game::resetPlayerPosition() {
    player.setX((int)originalPlayerX);
    player.setY((int)originalPlayerY);
    boat.setX((int)originalPlayerX);
    boat.setY((int)originalPlayerY);
}

void Game::clean() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (largeFont) {
        TTF_CloseFont(largeFont);
        largeFont = nullptr;
    }
    TTF_Quit();

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void Game::saveGameResult() {
    std::ofstream file("game_results.txt", std::ios::app);
    if (file.is_open()) {
        file << playerName << " | Score: " << score << " | Level: " << (currentLevel + 1) << "\n";
        file.close();
        std::cout << "Game result saved: " << playerName << " - Score: " << score << " - Level: " << (currentLevel + 1)
                  << "\n";
    } else {
        std::cerr << "Failed to open game_results.txt for writing\n";
    }
}

std::vector<GameResult> Game::loadGameResults() {
    std::vector<GameResult> results;
    std::ifstream file("game_results.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            // Parse line format: "name | Score: score | Level: level"
            size_t pos1 = line.find(" | Score: ");
            size_t pos2 = line.find(" | Level: ");

            if (pos1 != std::string::npos && pos2 != std::string::npos) {
                std::string name = line.substr(0, pos1);
                std::string scoreStr = line.substr(pos1 + 10, pos2 - pos1 - 10);
                std::string levelStr = line.substr(pos2 + 10);

                GameResult result;
                result.playerName = name;
                result.score = std::stoi(scoreStr);
                result.level = std::stoi(levelStr);
                results.push_back(result);
            }
        }
        file.close();
    }
    return results;
}

std::vector<GameResult> Game::getTop5Results() {
    std::vector<GameResult> allResults = loadGameResults();

    // Sort by score in descending order
    std::sort(allResults.begin(), allResults.end(), [](const GameResult& a, const GameResult& b) {
        return a.score > b.score;
    });

    // Return top 5 (or fewer if less than 5 results exist)
    if (allResults.size() > 5) {
        allResults.resize(5);
    }

    return allResults;
}

void Game::saveGameState() {
    // First, remove old save for this player
    std::ifstream infile("game_saves.txt");
    std::vector<std::string> lines;
    if (infile.is_open()) {
        std::string line;
        while (std::getline(infile, line)) {
            if (!line.empty()) {
                size_t nameEnd = line.find(" | Score: ");
                if (nameEnd != std::string::npos) {
                    std::string fileName = line.substr(0, nameEnd);
                    if (fileName != playerName) {
                        lines.push_back(line);
                    }
                }
            }
        }
        infile.close();
    }

    // Write back all lines except the old save for this player
    std::ofstream outfile("game_saves.txt");
    if (outfile.is_open()) {
        for (const auto& line : lines) {
            outfile << line << "\n";
        }
        // Append new save
        outfile << playerName << " | Score: " << score << " | Level: " << (currentLevel + 1)
                << " | Dead: 0 | PlayerX: " << player.getX() << " | PlayerY: " << player.getY()
                << " | BoatX: " << boat.getX() << " | BoatY: " << boat.getY()
                << " | InBoat: " << (player.getInBoat() ? 1 : 0) << "\n";
        outfile.close();
        std::cout << "Game state saved: " << playerName << " - Score: " << score << " - Level: " << (currentLevel + 1)
                  << " - Pos: (" << player.getX() << ", " << player.getY() << "), Boat Pos: (" << boat.getX() << ", "
                  << boat.getY() << ")\n";
    } else {
        std::cerr << "Failed to open game_saves.txt for writing\n";
    }
}

SavedGame Game::loadGameState(const std::string& name) {
    SavedGame result;
    result.playerName = name;
    result.score = 0;
    result.level = 0;
    result.isDead = false;
    result.playerX = 100;
    result.playerY = 100;
    result.boatX = 100;
    result.boatY = 100;
    result.inBoat = false;

    std::ifstream file("game_saves.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            // Parse line format: "name | Score: score | Level: level | Dead: isDead | PlayerX: x | PlayerY: y | BoatX:
            // bx | BoatY: by"
            size_t nameEnd = line.find(" | Score: ");
            if (nameEnd != std::string::npos) {
                std::string fileName = line.substr(0, nameEnd);
                if (fileName != name) continue;

                // Helper lambda to extract value between delimiters
                auto extractValue = [&line](const std::string& start_delim, const std::string& end_delim) -> int {
                    size_t start = line.find(start_delim);
                    if (start == std::string::npos) return 0;
                    start += start_delim.length();
                    size_t end = line.find(end_delim, start);
                    if (end == std::string::npos) end = line.length();
                    return std::stoi(line.substr(start, end - start));
                };

                result.score = extractValue(" | Score: ", " | Level: ");
                result.level = extractValue(" | Level: ", " | Dead: ");
                result.isDead = (extractValue(" | Dead: ", " | PlayerX: ") == 1);
                result.playerX = extractValue(" | PlayerX: ", " | PlayerY: ");
                result.playerY = extractValue(" | PlayerY: ", " | BoatX: ");
                result.boatX = extractValue(" | BoatX: ", " | BoatY: ");
                result.boatY = extractValue(" | BoatY: ", " | InBoat: ");
                result.inBoat =
                    (extractValue(" | InBoat: ", " | END") == 1); // END won't be found, so it takes rest of line

                file.close();
                return result;
            }
        }
        file.close();
    }
    return result;
}

bool Game::hasSavedGame(const std::string& name) {
    std::ifstream file("game_saves.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            size_t nameEnd = line.find(" | Score: ");
            if (nameEnd != std::string::npos) {
                std::string fileName = line.substr(0, nameEnd);
                if (fileName == name) {
                    file.close();
                    return true;
                }
            }
        }
        file.close();
    }
    return false;
}

void Game::deleteSavedGame(const std::string& name) {
    std::ifstream infile("game_saves.txt");
    std::ofstream outfile("game_saves_temp.txt");

    if (infile.is_open() && outfile.is_open()) {
        std::string line;
        while (std::getline(infile, line)) {
            if (line.empty()) continue;

            size_t nameEnd = line.find(" | Score: ");
            if (nameEnd != std::string::npos) {
                std::string fileName = line.substr(0, nameEnd);
                if (fileName != name) {
                    outfile << line << "\n";
                }
            }
        }
        infile.close();
        outfile.close();

        // Replace original file
        remove("game_saves.txt");
        rename("game_saves_temp.txt", "game_saves.txt");
    }
}

bool Game::playerHasDiedBefore(const std::string& name) {
    SavedGame saved = loadGameState(name);
    return saved.isDead;
}

void Game::addToBlacklist(const std::string& name) {
    std::ofstream file("blacklist.txt", std::ios::app);
    if (file.is_open()) {
        file << name << "\n";
        file.close();
        std::cout << "Player " << name << " added to blacklist.\n";
    } else {
        std::cerr << "Failed to open blacklist.txt for writing\n";
    }
}

bool Game::isBlacklisted(const std::string& name) {
    std::ifstream file("blacklist.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line == name) {
                file.close();
                return true;
            }
        }
        file.close();
    }
    return false;
}

std::vector<std::string> Game::loadBlacklist() {
    std::vector<std::string> blacklist;
    std::ifstream file("blacklist.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                blacklist.push_back(line);
            }
        }
        file.close();
    }
    return blacklist;
}