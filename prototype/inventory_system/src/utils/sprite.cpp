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
    DebugGUI::addDebugLog("Added SDL Texture: " + path, ErrorCode::SPRITE_ERROR);

    // Construct the object
    Sprite sprite;
    sprite.path = path;
    sprite.texture = texture;
    // add it to the list
    this->sprites.push_back(sprite);
}

/**
    @param index: The index of the sprite to change
    @param path:  the new path to change to
**/
void Sprites::changeSpritePathName(int index, std::string path)
{
    this->sprites[index].path = path;
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


void Sprites::drawSprite(Facing facing)
{
    /**
        so we have multiple sprites idk exactly how to handle this, probably by the direction of the player
        so in this situation this is a "Game Engine" so in things like godot they have a lsp that knows what
        the name of the sprite is but we dont have that so we will just have to run our engine and see what the
        name is to enter into here
    **/

    // in here since we set it: the 
    //      first row is the down facing
    //      second row is left the facing 
    //      third row is the right facing
    //      fourth row is the back facing <- Only has 4 frames

    /** before I can work on this I want to get the debugging better*/
    switch (facing)
    {
        case Facing::DOWN:
            // Draw the first row
            // float sprite_index = static_cast<int>(sprite.width / sprite.numFramesX);
            break;
        case Facing::LEFT:
            // Draw the second row
            break;
        case Facing::RIGHT:
            // Draw the third row
            break;
        case Facing::UP:
            // Draw the fourth row
            break;
        default:
            break;
    }

    /*
            for (Sprites::Sprite sprite : this->sprites)
            {
                DebugGUI::addDebugLog("Drawing Sprite: " + sprite.path, ErrorCode::NONE);
            }
    */
}
