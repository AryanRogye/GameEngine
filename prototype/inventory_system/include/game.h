#pragma once

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

#include "imgui.h"

#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "debug_gui.h"
#include "comfy_lib.h"


/** 
This File is a protype for my library for loading a tiled map
**/


#include "TSDL.h"
#include "entity/player.h"


class Game 
{
private:
    
    /** 
        SDL Variables
    **/
    SDL_Window      *window;
    SDL_Renderer    *renderer;
    SDL_Rect        gameViewport;
    
    /** 
        Game Variables
    **/
    TTF_Font                    *font;
    std::vector<SDL_Texture*>   fontNumbers;

    /** 
        Game Entities
    **/
    Player          *player;
    TSDL_TileMap    *map;

    float   gameScale;

    void initWindow();
    void initRenderer();
    void handleEvent(SDL_Event e, float dt);

    void loadMap();
    void loadFontNumbers();
    void renderGui();
    void drawMap();

    void initGui();

    bool running;


    int viewportWidth = 600;
    int viewportHeight = 500;
public:
    Game();
    void start_game();
};

