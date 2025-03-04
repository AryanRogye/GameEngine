#include "utils/collision.h"
// Required to be in this file to avoid circular imports
#include "debug_gui.h"
#include "entity/player.h"
#include "TSDL.h"
// End of last checked required includes

Collision::Collision(Player *player) 
{ 
    this->player = player; 
    this->setPlayerCollision();

    if (!fetchCollisionConfigs(this))
    {
        SM_WARN("No collision_data.ini found. Using default collision.");
        this->width = 200;
        this->height = 200;
        this->xOffset = 0;
        this->yOffset = 0;
    } else {
        std::cout << "Collision width: " << this->getWidth() << std::endl;
        std::cout << "Collision height: " << this->getHeight() << std::endl;
        std::cout << "Collision xOffset: " << this->getXOffset() << std::endl;
        std::cout << "Collision yOffset: " << this->getYOffset() << std::endl;
    }
}

// Getters
bool Collision::getShowingCollision() {return showCollision;}
SDL_Color Collision::getCollisionColor() {return collisionColor;}
Player* Collision::getPlayer() {return player;}
float Collision::getWidth() {return width;}
float Collision::getHeight() {return height;}
float Collision::getXOffset() {return xOffset;}
float Collision::getYOffset() {return yOffset;}
// Setters
void Collision::setShowingCollision(bool showCollision) {this->showCollision = showCollision;}
void Collision::setCollisionColor(SDL_Color collisionColor) {this->collisionColor = collisionColor;}
void Collision::setPlayer(Player *player) {this->player = player;}
void Collision::setWidth(float width) {this->width = width;}
void Collision::setHeight(float height) {this->height = height;}
void Collision::setXOffset(float xOffset) {this->xOffset = xOffset;}
void Collision::setYOffset(float yOffset) {this->yOffset = yOffset;}

// Methods
void Collision::setPlayerCollision()
{
    float scale = this->player->getPlayerScale();
    if (this->getPlayer()->getCamera())
    {
        DebugGUI::addDebugLog("Player camera is not null", ErrorCode::NONE);
        this->playerCollisionRect = {
            ((this->player->getX() - this->player->getCamera()->getX()) * scale) + (this->getXOffset() * scale),
            ((this->player->getY() - this->player->getCamera()->getY()) * scale) + (this->getYOffset() * scale),
            this->getWidth() * scale,
            this->getHeight() * scale
        };
    } 
    else 
    {
        DebugGUI::addDebugLog("Player camera is null", ErrorCode::COLLISION_ERROR);
    }
}

void Collision::drawPlayerCollision()
{
    // If we are not showing the collision then return
    if (!this->showCollision) return;

    this->setPlayerCollision();
    // Set the color of the renderer to the collision color
    SDL_SetRenderDrawColor(this->player->getRenderer(), this->collisionColor.r, this->collisionColor.g, this->collisionColor.b, this->collisionColor.a);
    // Draw the rect
    SDL_RenderDrawRectF(this->player->getRenderer(), &this->playerCollisionRect);
}


// TODO DOES NOT WORK
bool Collision::collidesWithMapLayer(TSDL_TileMap *tileMap, float scale)
{
    if (!tileMap || tileMap->layers.empty()) return false;

    // Get Player Bounds - scaled to match render position
    float playerLeft = (this->getPlayer()->getX() * scale) + (this->getXOffset() * scale);
    float playerRight = playerLeft + (this->getWidth() * scale);  // Use collision width instead of tile width
    float playerTop = (this->getPlayer()->getY() * scale) + (this->getYOffset() * scale);
    float playerBottom = playerTop + (this->getHeight() * scale);  // Use collision height instead of tile height

    // Convert to tile coordinates
    int startTileX = static_cast<int>(playerLeft / (tileMap->tileWidth * scale));
    int endTileX = static_cast<int>(playerRight / (tileMap->tileWidth * scale));
    int startTileY = static_cast<int>(playerTop / (tileMap->tileHeight * scale));
    int endTileY = static_cast<int>(playerBottom / (tileMap->tileHeight * scale));

    // Ensure we check at least one tile if we're exactly on a boundary
    if (startTileX == endTileX) endTileX++;
    if (startTileY == endTileY) endTileY++;

    // Clamp to map bounds
    startTileX = std::max(0, startTileX);
    endTileX = std::min(tileMap->width - 1, endTileX);
    startTileY = std::max(0, startTileY);
    endTileY = std::min(tileMap->height - 1, endTileY);

    // Debug visualization of collision boxes
    if (this->getShowingCollision()) {
        SDL_FRect debugRect = {
            playerLeft,
            playerTop,
            this->getWidth() * scale,
            this->getHeight() * scale
        };
        SDL_SetRenderDrawColor(this->getPlayer()->getRenderer(), 255, 0, 0, 128);
        SDL_RenderDrawRectF(this->getPlayer()->getRenderer(), &debugRect);
    }

    // wanna check if the player is going out of bounds
    if (playerLeft < 0 || playerRight > tileMap->width * tileMap->tileWidth * scale ||
        playerTop < 0 || playerBottom > tileMap->height * tileMap->tileHeight * scale) {
        return true;
    }

    // Check collision layer
    for (auto &layer : tileMap->layers) {
        if (layer.name == "Collision") {
            for (int y = startTileY; y <= endTileY; y++) {
                for (int x = startTileX; x <= endTileX; x++) {
                    int tileIndex = layer.data[x + y * layer.width];
                    if (tileIndex > 0) {
                        // Calculate tile bounds using the same scaling as drawMap
                        float tileLeft = (x * tileMap->tileWidth) * scale;
                        float tileRight = tileLeft + (tileMap->tileWidth * scale);
                        float tileTop = y * tileMap->tileHeight * scale;
                        float tileBottom = tileTop + (tileMap->tileHeight * scale);

                        // Debug visualization of tile collision boxes
                        if (this->getShowingCollision()) {
                            SDL_FRect tileRect = {tileLeft, tileTop, tileMap->tileWidth * scale, tileMap->tileHeight * scale};
                            SDL_SetRenderDrawColor(this->getPlayer()->getRenderer(), 0, 255, 0, 128);
                            SDL_RenderDrawRectF(this->getPlayer()->getRenderer(), &tileRect);
                        }

                        // Check for overlap
                        if (playerLeft < tileRight && playerRight > tileLeft &&
                            playerTop < tileBottom && playerBottom > tileTop) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}
