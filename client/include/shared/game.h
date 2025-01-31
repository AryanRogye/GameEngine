#include <SDL2/SDL.h>
#include "configs.h"
#include <iostream>
#include "client/client.h"
#include "player.h"
#include <unordered_map>
#include <thread>


class Game {
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool keep_window_open;
    Client *client;
public:
    Game();
    void start_game();
    void handleEvent(SDL_Event e);
    void initWindow();
    void initRenderer();
    void renderPlayer();
    void updateServer(float *oldX, float *oldY);
};
