#pragma once

#ifndef PLAYER_H
#define PLAYER_H
#include <iostream>
#include "configs.h"
#include "SDL2/SDL.h"
struct Position {
    float x;
    float y;
};

enum Facing {
    LEFT,
    RIGHT,
    FORWARD,
    BACKWARD
};

class Player {
private:
    int id;
    char name[32];
    int score;
    Position position;
    int width, height;
    float speed;
    bool isWalking;
    Facing facing;
    int spriteIndex;
    SDL_Rect worldHitbox;
public:
    // Constructor
    Player();
    // Setters
    void setScore(int score);
    void setPosition(float x, float y);
    void setPosition(Position position);
    void setX(float x);
    void setY(float y);
    void setName(const std::string& name);
    void setID(int id);
    void setWidth(int width);
    void setHeight(int height);
    void setSpeed(float speed);
    void setIsWalking(bool isWalking);
    void setSpriteIndex(int spriteIndex);
    void setFacing(Facing facing);


    // Getters
    int         getID();
    const char* getName();
    int         getScore();
    Position    getPosition();
    float       getX();
    float       getY();
    int         getWidth();
    int         getHeight();
    float       getSpeed();
    bool        getIsWalking();
    int         getSpriteIndex();
    SDL_Rect    getWorldHitbox();
    Facing      getFacing();

    // Debug
    void print();
    void printPosition();
    void printScore();
    void printName();
    void printId();
    void printAll();

    // Functions
    size_t serialize(uint8_t *buffer);
    void deserialize(const uint8_t *buffer, size_t *offset);
    void updateWorldHitbox( int offsetX, int offsetY );
};

#endif // !PLAYER_H
