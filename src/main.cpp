#include "Game.h"
#include <vector>

int main(int argc, char* argv[]) {

    Game game;

    if (game.init()) {
        game.run();
    }

    game.clean();

    return 0;
}
