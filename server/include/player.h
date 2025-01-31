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
public:
    // Constructor
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

    // Getters
    int         getId();
    const char* getName();
    int         getScore();
    Position    getPosition();
    float       getX();
    float       getY();
    int         getWidth();
    int         getHeight();
    float         getSpeed();

    // Debug
    void print();
    void printPosition();
    void printScore();
    void printName();
    void printId();
    void printAll();

    // Functions

};

#endif // !PLAYER_H
