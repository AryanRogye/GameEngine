#include "rendering/sprite.h"
#include "SDL2/SDL_render.h"
#include <_wchar.h>

Sprite::Sprite(std::string name, std::string path, int currentFrame, int frameCount)
{
    this->name = name;
    this->path = path;
    this->currentFrame = currentFrame;
    this->frameCount = frameCount;
}

void Sprite::renderSprite(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect destRect)
{
    if (!texture)
    {
        std::cout << "(Sprite) Texture Error: " << SDL_GetError() << std::endl;
    }
    if(SDL_RenderCopy(renderer, texture, &srcRect, &destRect) != 0)
    {
        std::cout << "(Sprite) Error: " << SDL_GetError() << std::endl;
    }
}
void Sprite::renderSprite(Sprite sprite, SDL_Renderer* renderer, SDL_Texture* texture,SDL_Rect rect, int width, int height)
{
    SDL_Rect srcRect = { sprite.getCurrentFrame() * width, 0, width, height };
    SDL_RenderCopyEx(renderer, texture, &srcRect, &rect, 0.0, NULL, SDL_FLIP_NONE);
}

void Sprite::fillRectVector(std::vector<SDL_Rect>& rect, int atlasWidth, int atlasHeight, int gridWidth, int gridHeight)
{
    if (gridHeight == -1)
    {
        gridHeight = gridWidth;
    }

    int tilesPerRow = atlasWidth / gridWidth;
    int tilesPerCol = atlasHeight / gridHeight;

    for (int y = 0; y < tilesPerCol; y++) {
        for (int x = 0; x < tilesPerRow; x++) {
            // Fill With Fake Tiles
            SDL_Rect tileRect {
                x * gridWidth,  // Calculate actual pixel position
                y * gridHeight,
                gridWidth,
                gridHeight 
            };
            rect.push_back(tileRect);
        }
    }
}
// Version that accepts a precomputed destination rectangle
void Sprite::renderSprite(Sprite sprite, SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect &destRect, Player* player)
{
    SDL_Rect srcRect = { sprite.getCurrentFrame() * destRect.w, 0, destRect.w, destRect.h };
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (player->getFacing() == Facing::RIGHT) { flip = SDL_FLIP_HORIZONTAL; }
    else { flip = SDL_FLIP_NONE; }
    if (SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0.0, NULL, flip) != 0)
    {
        std::cout << "(Sprite) Error: " << SDL_GetError() << std::endl;
    }
}

// Overloaded version that calculates the destination rectangle internally
void Sprite::renderSprite(Sprite sprite, SDL_Renderer* renderer, SDL_Texture* texture, int width, int height, Player* player, int camX, int camY)
{
    SDL_Rect srcRect = { sprite.getCurrentFrame() * width, 0, width, height };
    SDL_Rect destRect = { static_cast<int>(player->getX() - camX), static_cast<int>(player->getY() - camY), PLAYER_WIDTH, PLAYER_HEIGHT };
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (player->getFacing() == Facing::RIGHT) { flip = SDL_FLIP_HORIZONTAL; }
    else { flip = SDL_FLIP_NONE; }

    if (SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0.0, NULL, flip) != 0)
    {
        std::cout << "(Sprite) Error: " << SDL_GetError() << std::endl;
    }
}
void Sprite::renderSprite(Sprite sprite, SDL_Renderer* renderer, SDL_Texture* texture, int width, int height, Player* player, SDL_Rect& hitbox)
{
    int windowWidth = WIDTH; 
    int windowHeight = HEIGHT;
    int camX = player->getX() + static_cast<int>(player->getWidth() / 2 - windowWidth / 2);
    int camY = player->getY() + static_cast<int>(player->getHeight() / 2 - windowHeight / 2);
    // Define A Source Rectangle
    SDL_Rect srcRect = {sprite.getCurrentFrame() * width, 0, width, height};
    SDL_Rect destRect = { (int)player->getX() - camX, (int)player->getY() - camY, PLAYER_WIDTH, PLAYER_HEIGHT};

    SDL_Rect worldBox = player->getWorldHitbox();
    hitbox = {
        worldBox.x - camX,  // Convert world coordinates to screen space
        worldBox.y - camY,
        worldBox.w,
        worldBox.h
    };

    // Determine if the sprite should be flipped horizontally
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (player->getFacing() == Facing::RIGHT) { flip = SDL_FLIP_HORIZONTAL; }
    else { flip = SDL_FLIP_NONE; }
    // Render the sprite with the flip
    if (SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0.0, NULL, flip) != 0)
    {
        std::cout << "(Sprite) Error: " << SDL_GetError() << std::endl;
    }
}

void Sprite::renderDebugHitbox(SDL_Renderer* renderer, const SDL_Rect& hitbox) {
    // Set color to red for hitbox
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &hitbox);

    // Reset render color back to default
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

// Getters
std::string Sprite::getName() { return name; }
std::string Sprite::getPath(){ return path; }
int         Sprite::getCurrentFrame(){ return currentFrame; }
int         Sprite::getFrameCount(){ return frameCount; }

// Setters
void Sprite::setName(std::string name){ this->name = name; }
void Sprite::setPath(std::string path){ this->path = path; }
void Sprite::setCurrentFrame(int currentFrame){ this->currentFrame = currentFrame; }
void Sprite::setFrameCount(int frameCount){ this->frameCount = frameCount; }
