#pragma once

#ifndef SPRITE_H
#define SPRITE_H

#include <vector>
#include <string>

#include "SDL2/SDL.h"
#include <SDL_image.h>

class Player;

class Sprites
{
    public:
        struct Sprite
        {
            std::string path;
            SDL_Texture *texture;
            int width;
            int height;
            int numFramesX;
            int numFramesY;
            int currentFrame;
        };
    private:

        std::vector<Sprite> sprites;

    public:
    Sprites(Player *player);

    // Getters
    std::vector<Sprite> getSpritePaths();
    SDL_Texture * getTextures(int index);
    // Setters

    void changeSpritePathName(int index, std::string path);

    void addSpritePath(std::string path);
    void addSpriteFramesX(int index, int numFramesX);
    void addSpriteFramesY(int index, int numFramesY);

    void addSprite(Sprite sprite);
    void removeSpritePath(int index);
    void clearSpritePaths();
    void queryTexureDimensions();
};

#endif
