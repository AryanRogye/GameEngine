#pragma once

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

#include "imgui.h"

#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "gui_values.h"


/** 
This File is a protype for my library for loading a tiled map
**/


#include "TSDL.h"


class Game 
{
private:

    std::vector<bool> layerInfo;
    GuiValues guiValues;

    float mapScale = 3;

    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    TSDL_TileMap map;
    std::vector<SDL_Texture*> fontNumbers;

    void initWindow();
    void initRenderer();
    void handleEvent(SDL_Event e);

    void loadMap();
    void loadFontNumbers();
    void renderGui();
    void drawMap();

    void initGui();

    bool running;
public:
    Game();
    void start_game();
};

