#pragma once
#include "Character.h"
#include "PerlinNoise.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <string>
#include <vector>

struct Level {
    int enemyCount;
    int trashCount;
    int friendCount;
    float enemySpeed;
    float trashSpeed;
    float playerSpeed;
    float friendSpeed;
    int seed;
};

struct GameResult {
    std::string playerName;
    int score;
    int level;
};

struct SavedGame {
    std::string playerName;
    int score;
    int level;
    bool isDead;
};

enum GameState { MAIN_MENU, LEVEL_SELECT, NAME_INPUT, PLAYING, DEATH_SCREEN, GAME_OVER };

class Game {
public:
    bool init();
    void loadLevel(int levelIndex);
    void run();
    void handleEvents();
    void update();
    void render();
    void clean();
    void setdeltaTime(float dt) {
        deltaTime = dt;
    }
    float getdeltaTime() const {
        return deltaTime;
    }

    float getlastTime() const {
        return lastTime;
    }

    void setlastTime(Uint32 time) {
        lastTime = time;
    }

private:
    SDL_Window* win = nullptr;
    SDL_Renderer* ren = nullptr;
    bool running = false;

    static const int GRID_SIZE = 75;
    static const int SQUARE_SIZE = 30;
    int windowWidth = 1920;
    int windowHeight = 1080;

    std::vector<std::vector<int>> grid;

    static constexpr double frequency = 0.02;  // lower = bigger continents
    static constexpr double threshold = 0.525; // higher = less land
    PerlinNoise noise{16305};                  // Fixed seed for consistent results

    Player player;
    std::vector<Enemy> enemies;
    std::vector<Friend> friends;
    std::vector<Trash> trash;
    Boat boat;

    TTF_Font* font = nullptr;
    TTF_Font* largeFont = nullptr;

    int score = 0;
    int enemiesRemaining = 0;
    int trashRemaining = 0;
    int friendsRemaining = 0;
    Uint32 lastTime = 0;
    float deltaTime = 0.0f;

    std::vector<Level> levels;
    int currentLevel = 0;

    // Game states
    GameState gameState = MAIN_MENU;
    std::string playerName = "";
    int selectedLevel = 0;
    float deathScreenTimer = 0.0f;
    static constexpr float DEATH_SCREEN_DURATION = 3.0f;

    // Store original player position for respawn
    float originalPlayerX = 0.0f;
    float originalPlayerY = 0.0f;

    // Resume game flag
    bool isResumedGame = false;
    bool resumedGameIsDead = false;

    // Menu functions
    void handleMenuEvents();
    void handleNameInputEvents();
    void handleLevelSelectEvents();
    void renderMainMenu();
    void renderNameInput();
    void renderLevelSelect();
    void renderDeathScreen();
    void resetPlayerPosition();

    // File I/O functions
    void saveGameResult();
    std::vector<GameResult> loadGameResults();

    // Save/Load game state functions
    void saveGameState();
    SavedGame loadGameState(const std::string& name);
    bool hasSavedGame(const std::string& name);
    void deleteSavedGame(const std::string& name);
    bool playerHasDiedBefore(const std::string& name);
};