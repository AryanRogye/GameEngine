#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <iostream>

class Texture
{
public:
    static bool loadTexture(std::string path, SDL_Texture** texture, SDL_Renderer* renderer);
};

