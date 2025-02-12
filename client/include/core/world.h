#pragma once

#include "client/client.h"
#include "rendering/texture.h"
#include "core/map_loader.h"
#include "block.h"
#include "configs.h"
#include "rendering/sprite.h"
#include <SDL2/SDL.h>
#include <memory>
#include "rendering/ui.h"

#ifndef WORLD_H
#define WORLD_H

class World {
private:
    int screenWidth;
    int screenHeight;

    bool commandMode = false;
    bool sentMap = false;
    bool allowBreak = false;
    std::string commandInput = "";
    SDL_Rect commandBox;
    int command_box_width = WIDTH;
    int command_box_height = 30;
    int command_box_x = 0;
    int command_box_y = (HEIGHT - 20) - command_box_height;

    /** Renderer/Window **/
    SDL_Window* window;
    SDL_Renderer* renderer;
    // Blocks
    BlockFactory bf;

    /** 
     * Sprite Handling 
     **/
    Uint32 lastFrameTime = 0;
    const Uint32 frameDelay = 180;  // How Long Each Frame of a Sprite Lasts
    SDL_Rect hitbox;
    Sprite playerIdleSprite;        // Sprite for Player Idle
    Sprite playerRunSprite;         // Sprite for Player Running
    /** 
     * Player Stuff This Should Be Loaded At The Start 
     *      - Later On May Add Skins For Characters
     **/
    SDL_Texture* playerIdleTexture;     // Texture for Player
    SDL_Texture* playerRunTexture;    // Texture for Running
    // For Sending to the server
    bool sendPlayerTexture = false;
    
    // House Stuff
    bool enterHouse;                // If Player Is Entering House
    SDL_Texture* house_one_texture;
    SDL_Rect house_one_rect;

    /** 
     * TileSet Atlas 
     *      -- Tiles are 16x16 Grid of Tiles 208x208 Pixels
     *      -- The Way This Works is that each tile represents
     *              a index in the mapData vector starting at 1
     *      -- Special Tiles Have a Special Index meaning negative
     *      -- src/core/map_loader.cpp function parseFile assigns
     *              the index to the tile
     **/
    SDL_Texture * tileAtlasTexture;

    std::vector<SDL_Rect> tiles;
    std::vector<std::vector<int>> mapData;  // Stores Index of Tile In TileSet

    /** 
     * Font Rendering
     *      -- Showing Different Things on the screen
     *      -- Fonts Are Made inside the shared/start_game.cpp
     *      -- the texture and vector is getting loaded in from 
     *              start_game originaly then passed to game then here
     **/
    SDL_Texture* font_texture;
    std::vector<SDL_Rect> fonts;

    MapLoader mapLoader;
    std::unique_ptr<Client> client;
    bool keep_window_open = true;
    std::string currentPath;
public:
    World(
        SDL_Renderer* renderer, 
        SDL_Window* window
    );
    World();

    void handleEvent(SDL_Event e);

    void updateServer(float *oldX, float *oldY);

    void setupWorld();
    void loadFont();
    void updateAndRender();

    void renderMap();
    void renderHouse(int x, int y, int width, int height);
    void renderPlayer();
    void renderOtherPlayers();
    void renderCommandInput();

    // Interactions With The Map
    void checkIfPosIsEnterable();
    void checkIfPosIsBreakable();

    // Color functions
    void drawGreen();
    void drawBlue();
    void drawRed();
    void drawYellow();

    std::unordered_map<SDL_Keycode, bool> keyStates;  // Track held keys


    Uint32 lastTime;
    const Uint8 *state;
    void handlePlayerMovement(const Uint8* state, float dt);
    void handleCommandInput(SDL_Event& e);


    // We Want Instance of Client Getter
    Client* getClient();
};

#include "shared/commands.h"

#endif // WORLD_H


