#pragma once

#ifndef UTILS_COLLISION_H
#define UTILS_COLLISION_H

#include <SDL2/SDL.h>
#include "comfy_lib.h"

// Since we dont want circular imports we need this here
// inside the implimentation file include the player.h
class Player;
class TSDL_TileMap;

/**
    This File Works Simple When A Player Is Created This is attached to them
 **/
class Collision {
private:
    // By Default We Dont Want to show the collision box
    bool showCollision = true;
    // Default Collision Color is Red
    SDL_Color collisionColor = {255, 0, 0, 255};
    Player* player;
    SDL_FRect playerCollisionRect;

    float width;
    float height;
    float xOffset;
    float yOffset;

public:
    // Constructor
    Collision(Player *player);
    // Getters
    bool getShowingCollision();
    SDL_Color getCollisionColor();
    Player *getPlayer();
    float getWidth();
    float getHeight();
    float getXOffset();
    float getYOffset();
    // Setters
    void setShowingCollision(bool showCollision);
    void setCollisionColor(SDL_Color collisionColor);
    void setPlayer(Player *player);
    void setWidth(float width);
    void setHeight(float height);
    void setXOffset(float xOffset);
    void setYOffset(float yOffset);
    // Methods

    void setPlayerCollision();
    void drawPlayerCollision();

    // Checking Collision With Map Layer
    bool collidesWithMapLayer(TSDL_TileMap *tileMap,float scale);
};

#endif
