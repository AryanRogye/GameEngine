#pragma once

#ifndef __MAINVIEW_H__
#define __MAINVIEW_H__

#include <SDL2/SDL.h>

class MainView
{
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* gameTexture;
public:
    MainView();
    ~MainView();
    
    void render();
    void update();
};

#endif
