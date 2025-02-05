#include "rendering/sprite.h"

Sprite::Sprite(std::string name, std::string path, int currentFrame, int frameCount)
{
    this->name = name;
    this->path = path;
    this->currentFrame = currentFrame;
    this->frameCount = frameCount;
}
void Sprite::renderSprite(Sprite sprite, SDL_Renderer* renderer, SDL_Texture* texture,SDL_Rect rect, int width, int height)
{
    SDL_Rect srcRect = { sprite.getCurrentFrame() * width, 0, width, height };
    SDL_RenderCopyEx(renderer, texture, &srcRect, &rect, 0.0, NULL, SDL_FLIP_NONE);
}

void Sprite::renderSprite(Sprite sprite, SDL_Renderer* renderer, SDL_Texture* texture, int width, int height, Player* player)
{
    int windowWidth = WIDTH; 
    int windowHeight = HEIGHT;
    int camX = player->getX() + static_cast<int>(player->getWidth() / 2 - windowWidth / 2);
    int camY = player->getY() + static_cast<int>(player->getHeight() / 2 - windowHeight / 2);
    // Define A Source Rectangle
    SDL_Rect srcRect = {sprite.getCurrentFrame() * width, 0, width, height};
    SDL_Rect destRect = { (int)player->getX() - camX, (int)player->getY() - camY, PLAYER_WIDTH, PLAYER_HEIGHT};
    // Determine if the sprite should be flipped horizontally
    SDL_RendererFlip flip = player->getFacingRight() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    // Render the sprite with the flip
    SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0.0, NULL, flip);
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
