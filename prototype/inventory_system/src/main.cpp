#include <iostream>
#include "game.h"
#include "engine/GameEngine.h"

int main()
{
    /*Game game = Game();*/
    /*game.start_game();*/

    GameEngine engine = GameEngine();
    engine.run();

    return 0;
}
