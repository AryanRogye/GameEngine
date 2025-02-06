#pragma once

#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include <iostream>
#include "configs.h"
#include "shared/player.h"
#include <SDL2/SDL.h>
#include <SDL_image.h>
class Sprite
{
private:
    std::string name;
    std::string path;
    int currentFrame;
    int frameCount;
public:
    Sprite(std::string name, std::string path, int currentFrame, int frameCount);

    static void renderSprite(Sprite sprite, SDL_Renderer* renderer, SDL_Texture* texture, int width, int height, Player* player);
    static void renderSprite(Sprite sprite, SDL_Renderer* renderer, SDL_Texture* texture,SDL_Rect rect, int width, int height);
    static void renderSprite(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect destRect);
    
    /*int tileSize = 16;*/
    /*int atlasWidth = 208;*/
    /*int atlasHeight = 208;*/
    static void fillRectVector(std::vector<SDL_Rect>& rects,int atlasWidth, int atlasHeight, int tileWidth, int tileHeight = -1);

    // Getters
    std::string getName();
    std::string getPath();
    int getCurrentFrame();
    int getFrameCount();

    // Setters
    void setName(std::string name);
    void setPath(std::string path);
    void setCurrentFrame(int currentFrame);
    void setFrameCount(int frameCount);
};

#endif // SPRITE_H
