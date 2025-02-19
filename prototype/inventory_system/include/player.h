#pragma once

#include <SDL_render.h>
#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Vec2.h"
#include "comfy_lib.h"
#include "debug_gui.h"

class Player
{
private:

    enum class PlayerState
    {
        IDLE,
        WALKING,
        ATTACK
    };

    int health;
    int damage;
    int level;
    int experience;
    int maxHealth;
    PlayerState state;

    Vec2 position;
    Vec2 velocity;
    float acceleration;
    float maxSpeed;
    float friction;
    bool keysPressed[4]; // Track W,A,S,D states

    SDL_Renderer *renderer;
public:
    Player();

    // Getters
    float getX();
    float getY();
    int getHealth();
    int getDamage();
    int getLevel();
    int getExperience();
    int getMaxHealth();
    SDL_Renderer* getRenderer();
    int getSpeed();
    float getAcceleration();
    float getMaxSpeed();
    float getFriction();
    float getVelocityX();
    float getVelocityY();

    // Setters
    void setX(float x);
    void setY(float y);
    void setHealth(int health);
    void setDamage(int damage);
    void setLevel(int level);
    void setExperience(int experience);
    void setMaxHealth(int maxHealth);
    void setRenderer(SDL_Renderer *renderer);
    void setSpeed(int speed);
    void setAcceleration(float acceleration);
    void setMaxSpeed(float maxSpeed);
    void setFriction(float friction);
    void setVelocityX(float velocityX);
    void setVelocityY(float velocityY);
    // Methods
    void loadPlayer();
    void drawPlayer(float dt, float scale);

    void handleInput(SDL_Event &event, float dt);
    void update(float dt);
};

#endif
