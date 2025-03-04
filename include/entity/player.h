#pragma once

#include "utils/camera.h"
#include "utils/collision.h"
#include "utils/sprite.h"
#include <SDL_render.h>
#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Vec2.h"
#include "comfy_lib.h"
#include "debug_gui.h"

class Player : public Entity
{
public:
    enum class PlayerState {
        IDLE,
        WALKING,
        ATTACK,
        COLLIDING
    };
private:
    int             health;
    int             damage;
    int             level;
    int             experience;
    int             maxHealth;
    PlayerState     state;

    float           width;
    float           height;
    Vec2            position;
    Vec2            velocity;
    float           acceleration;
    float           maxSpeed;
    float           friction;
    bool            keysPressed[4]; // Track W,A,S,D states
    float           playerScale;

    // Tools The Player Needs
    TSDL_TileMap    *tileMap;
    Collision       *collision;
    Camera          *camera;
    Sprites         *sprite;

    SDL_Renderer    *renderer;

public:
    Player();

    // Getters
    float getX() override;
    float getY() override;
    float getWidth() override;
    float getHeight() override;
    int getHealth();
    int getDamage();
    int getLevel();
    int getExperience();
    int getMaxHealth();
    SDL_Renderer *getRenderer();
    float getAcceleration();
    float getMaxSpeed() override;
    float getFriction();
    float getVelocityX();
    float getVelocityY();
    Collision *getCollision();
    Camera *getCamera();
    Sprites *getSprite() override;
    TSDL_TileMap *getTileMap();
    PlayerState getState();
    float getPlayerScale();

    // Setters
    void setX(float x);
    void setY(float y);
    void setHealth(int health);
    void setDamage(int damage);
    void setLevel(int level);
    void setExperience(int experience);
    void setMaxHealth(int maxHealth);
    void setRenderer(SDL_Renderer *renderer) override;
    void setSpeed(int speed);
    void setAcceleration(float acceleration);
    void setMaxSpeed(float maxSpeed) override;
    void setFriction(float friction);
    void setVelocityX(float velocityX);
    void setVelocityY(float velocityY);
    void setCollision(Collision *collision);
    void setCamera(Camera *camera);
    void setTileMap(TSDL_TileMap *tileMap);
    void setState(PlayerState state);
    void setPlayerScale(float scale);
    void setSprite(Sprites *sprite) override;
    // Methods
    void loadPlayer();
    void draw(float dt, float scale) override;

    void handleInput(SDL_Event &event, float dt);
    void update(float dt) override;
    bool isMoving();
};

#endif
