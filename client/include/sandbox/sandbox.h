#pragma once
#ifndef SANDBOX_H
#define SANDBOX_H

/** 
  * This is A Dynamic Libary Cuz It has too many dependencies 
  * And This Wont Always Be Called
 **/

/**
  *
  * This is a sandbox for testing purposes its a bit buggy but right now all that matters
  * is getting a working copy down its pretty advanced its kind of like a tilemap engine
  * built into the game engine I dont think its too heavy weight we have to see tho lol
  *
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
    /** First Thing Will Be Opening The File Or Choosing a File **/
    std::string filePath;   /** This is the current file path of this file **/
    /** Button To Oepn File **/
    SDL_Rect openFileButton;
    int o_file_button_width = 200;
    int o_file_button_height = 50;
    int o_file_button_x = 10;
    int o_file_button_y = 10;

    SDL_Window *window;
    SDL_Renderer *renderer;
    bool isRunning;
    static bool sdlInitialized;
    void initWindow();
    void initRenderer();
    bool checkButtonClicked(SDL_Rect buttonRect, int mouseX, int mouseY);

public:
    SandBox();
    ~SandBox();
    void loadSandBox();
    void handleEvent(SDL_Event e);
    void renderOpenFile();
    void cleanup();
};



extern "C" {
EXPORT SandBox* createSandBox();
EXPORT void runSandBox(void* instance);
}

#endif // SANDBOX_H
