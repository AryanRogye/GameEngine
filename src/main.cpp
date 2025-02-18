#include "shared/start_game.h"
#include "configs.h"

int main()
{
    initializePaths();
    StartGame start_game = StartGame();
    start_game.start_screen();
}
