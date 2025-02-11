#pragma once

#ifndef PLAYER_H
#define PLAYER_H
#include <iostream>
struct Position {
    float x;
    float y;
};

class Player {
private:
    int id;
    char name[32];
    int score;
    Position position;
    int width, height;
    float speed;
    bool facingRight;
    bool isWalking;
    int spriteIndex;
public:
    // Constructor
    Player() = default;
    Player(int id, const char* name);
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
    void setFacingRight(bool facingRight);
    void setIsWalking(bool isWalking);
    void setSpriteIndex(int spriteIndex);

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
    bool        getFacingRight();
    bool        getIsWalking();
    int         getSpriteIndex();

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
};

#endif // !PLAYER_H
