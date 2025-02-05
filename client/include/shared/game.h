#include <SDL2/SDL.h>
#include "configs.h"
#include <SDL2/SDL_render.h>
#include <iostream>
#include "player.h"
#include <memory>
#include <unordered_map>
#include <thread>
#include <SDL_image.h>
#include <string>
#include <fstream>
#include <SDL2/SDL_render.h>
#include <SDL_hints.h>
#include <core/map_loader.h>
#include <core/block.h>
#include "core/sprite.h"
#include "world.h"


class Game {
private:
    std::unique_ptr<World> world;

    BlockFactory bf;

    SDL_Window *window;
    SDL_Renderer *renderer;
    Uint32 lastFrameTime = 0;
    const Uint32 frameDelay = 180;

    // Player Stuff
    SDL_Texture* playerTexture;
    SDL_Texture* runningTexture;

    // House Stuff
    bool enterHouse;
    SDL_Texture* house_one_texture;
    SDL_Rect house_one_rect;

    // Tileset Atlas and its Cells
    SDL_Texture * tileAtlasTexture;
    std::vector<SDL_Rect> tiles;
    // Keep Track of MapData
    std::vector<std::vector<int>> mapData;

    bool keep_window_open;
    Client *client;
public:
    Game(Sprite playerIdle, Sprite playerRun);
    void start_game();
    void handleEvent(SDL_Event e);
    void initWindow();
    void initRenderer();
    void renderPlayer();
    void loadPlayerSprites();
    void loadHouseSprites(std::string filePath);
    void updateServer(float *oldX, float *oldY);
    void renderOtherPlayers();

    // Interactions With The Map
    void checkIfPosIsEnterable();

    // Read File Map
    void parseMap(std::string filePath);
    // Load Tileset Image
    void loadTileset(std::string filePath);
    void initializeTiles();

    // Render The Map
    void renderMap();
    void renderHouse(int x, int y, int width, int height);
    
    // Color functions
    void drawGreen();
    void drawBlue();
    void drawRed();
    void drawYellow();
};

