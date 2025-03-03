#pragma once
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "SDL2/SDL.h"
#include "SDL_image.h"
#include "game.h"
#include <iostream>

class GameEngine
{
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    SDL_Texture* gameTexture = nullptr;
    int lastWidth = 0, lastHeight = 0;
    Game* game = nullptr;


    bool running_engine = false;

    void render();
    void handleEvents();

    void CreateDockLayout();
    void RenderGameViewport();
public:
    GameEngine();
    ~GameEngine();

    void run();
};


#endif
