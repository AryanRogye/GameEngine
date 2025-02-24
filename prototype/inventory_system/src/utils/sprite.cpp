#include "utils/sprite.h"
#include "comfy_lib.h"
#include "entity/player.h"
#include "debug_gui.h"
#include <SDL_surface.h>

Sprites::Sprites(Player *player)
{
    fetchSpritesConfigs(this);

    // Once we have the sprites, the width and the height is not saved so we can easily
    // query it
    this->queryTexureDimensions();

    if (this->sprites.empty())
    {
        SM_WARN("No sprites found");
        // init a empty sprite (maybe the gui will handle this)
        this->sprites = std::vector<Sprites::Sprite>();
    }
}

void Sprites::queryTexureDimensions()
{
    for (int i = 0; i < this->sprites.size(); i++)
    {
        int w, h;
        SDL_QueryTexture(this->sprites[i].texture, NULL, NULL, &w, &h);
        this->sprites[i].width = w;
        this->sprites[i].height = h;
    }
    DebugGUI::addDebugLog("Queried texture dimensions", ErrorCode::SPRITE_ERROR);
}

void Sprites::addSprite(Sprite sprite)
{
    this->sprites.push_back(sprite);
}

// Getters
std::vector<Sprites::Sprite> Sprites::getSpritePaths()
{
    return this->sprites;
}

SDL_Texture *Sprites::getTextures(int index)
{
    return this->getSpritePaths()[index].texture;
}
// Setters
void Sprites::addSpritePath(std::string path)
{
    // Getting the surface
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface)
    {
        SM_WARN("Failed to load surface: {0}");
        return;
    }
    // getting the texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(SDL_GetRenderer(SDL_GetWindowFromID(1)), surface);
    if (!texture)
    {
        SDL_FreeSurface(surface);
        SM_WARN("Failed to load texture: {0}");
        return;
    }
    DebugGUI::addDebugLog("Added SDL Texture\n" + path, ErrorCode::SPRITE_ERROR);

    // Construct the object
    Sprite sprite;
    sprite.path = path;
    sprite.texture = texture;
    // add it to the list
    this->sprites.push_back(sprite);
}

void Sprites::addSpriteFramesX(int index, int numFramesX)
{
    this->sprites[index].numFramesX = numFramesX;
}
void Sprites::addSpriteFramesY(int index, int numFramesY)
{
    this->sprites[index].numFramesY = numFramesY;
}

void Sprites::removeSpritePath(int index)
{
    this->sprites.erase(this->sprites.begin() + index);
}

void Sprites::clearSpritePaths()
{
    this->sprites.clear();
}
