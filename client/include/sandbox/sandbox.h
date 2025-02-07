#pragma once
#include <_wchar.h>
#include <vector>
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
#include <SDL_image.h>
#include <SDL_render.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <map>
#include "rendering/sprite.h"
#include "rendering/texture.h"
#include "rendering/ui.h"
#include <algorithm>
#include "configs.h"



class SandBox 
{
private:
    /** 
     * Font Rendering
     *      -- Showing Different Things on the screen
     *      -- Fonts Are Made inside the shared/start_game.cpp
     *      -- the texture and vector is getting loaded in from 
     *              start_game originaly then passed to game then here
     **/
    SDL_Texture* font_texture;
    std::vector<SDL_Rect> fonts;
    /** First Thing Will Be Opening The File Or Choosing a File **/
    std::string currentPath;   /** This is the current file path of this file **/
    /** Button To Oepn File **/
    SDL_Rect openFileButton;
    /** We Will Keep A Vector Of Rects representing the files **/
    std::vector<SDL_Rect> fileRects;
    int o_file_button_width = 200;
    int o_file_button_height = 50;
    int o_file_button_x = 10;
    int o_file_button_y = 10;

    SDL_Rect closeWindowButton;
    int c_window_button_width = 50;
    int c_window_button_height = 50;
    int c_window_button_x = 220;
    int c_window_button_y = 10;

    bool showFiles = false;
    std::map<std::string, char> fileMap;

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
    void loadFont();
    void handleEvent(SDL_Event e);
    void cleanup();

    void handleOpenFileButtonClick();
    void handleFileClicked(int mouseX, int mouseY);

    void renderOpenFile();
    void renderFiles();
    void renderCloseWindow();
    void renderFileClicked(std::string fileName);
};



extern "C" {
EXPORT SandBox* createSandBox();
EXPORT void runSandBox(void* instance);
}

#endif // SANDBOX_H
