#include "world.h"

#include <SDL2/SDL.h>
#include "core/world.h"
#include "rendering/sprite.h"
#include <SDL_render.h>
#include <iostream>


class Game {
private:
    std::unique_ptr<World> world;

    SDL_Window *window;
    SDL_Renderer *renderer;
    void initWindow();
    void initRenderer();
public:
    Game(Sprite playerIdle, Sprite playerRun);
    void start_game();

};

