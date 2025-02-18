#include "rendering/texture.h"

bool Texture::loadTexture(std::string path, SDL_Texture** texture, SDL_Renderer* renderer)
{
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (!loadedSurface)
    {
        std::cout << "Failed To Load Sprite: " << path << std::endl;
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    *texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    if (!*texture)
    {
        std::cout << "Failed To Create Texture: " << path << std::endl;
        return false;
    }

    SDL_FreeSurface(loadedSurface);

    return true;
}
