#include <SDL2/SDL.h>
#include "configs.h"
#include <SDL2/SDL_render.h>
#include <iostream>
#include "client/client.h"
#include "player.h"
#include <unordered_map>
#include <thread>
#include <SDL_image.h>
#include <string>
#include <fstream>
#include <SDL2/SDL_render.h>
#include <SDL_hints.h>
#include <core/map_loader.h>
#include <core/block.h>


class Game {
private:
    SDL_Window *window;
    SDL_Renderer *renderer;

    // Tileset Atlas and its Cells
    SDL_Texture * tileAtlasTexture;
    std::vector<SDL_Rect> tiles;
    // Keep Track of MapData
    std::vector<std::vector<int>> mapData;

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
    void renderOtherPlayers();

    // Read File Map
    void parseMap(std::string filePath);
    // Load Tileset Image
    void loadTileset(std::string filePath);
    void initializeTiles();

    // Render The Map
    void renderMap();
    
    // Color functions
    void drawGreen();
    void drawBlue();
    void drawRed();
    void drawYellow();
};

