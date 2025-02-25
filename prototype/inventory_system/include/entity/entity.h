#pragma once

#ifndef ENTITY_H
#define ENTITY_H

#include "utils/sprite.h"
#include "Vec2.h"
#include <SDL2/SDL.h>
#include <string>

class Entity {
protected:
    float width;
    float height;
    std::string     name;
    Vec2            position;
    float           maxSpeed;
    Sprites         *sprite;
    SDL_Renderer    *renderer;

public:
    virtual ~Entity() = default;

    // Common Getters
    virtual float getX() { return position.x; }
    virtual float getY() { return position.y; }
    virtual float getWidth() { return width; }
    virtual float getHeight() { return height; }

    virtual Vec2 getPosition() { return position; }
    virtual float getMaxSpeed() { return maxSpeed; }
    virtual Sprites* getSprite() { return sprite; }
    virtual std::string getName() { return name; }

    // Common Setters
    virtual void setPosition(float x, float y) { position.x = x; position.y = y; }
    virtual void setMaxSpeed(float spd) { maxSpeed = spd; }
    virtual void setSprite(Sprites *spr) { sprite = spr; }
    virtual void setRenderer(SDL_Renderer *rend) { renderer = rend; }

    // Core Methods
    virtual void update(float dt) = 0; // Must be implemented by derived classes
    virtual void draw(float dt, float scale) = 0;
};

#endif
