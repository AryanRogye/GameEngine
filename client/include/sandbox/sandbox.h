#pragma once
#ifndef SANDBOX_H
#define SANDBOX_H

/** 
  * This is A Dynamic Libary Cuz It has too many dependencies 
  * And This Wont Always Be Called
 **/

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <SDL2/SDL.h>
#include <SDL_render.h>
#include <iostream>


class SandBox 
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool isRunning;
    void initWindow();
    void initRenderer();
public:
    SandBox();
    void loadSandBox();
    void handleEvent(SDL_Event e);
};

extern "C" {
EXPORT SandBox* createSandBox();
EXPORT void runSandBox(void* instance);
}

#endif // SANDBOX_H
