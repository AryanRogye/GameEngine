#pragma once

#include <iostream>

class Player{
private:
    char name[32];
    int id;
    int score;
public:
    Player();
    Player(char* name, int id, int score);
    Player(char* name, int id);
    Player(char* name);

    // Getters
    char* getName();
    int getId();
    int getScore();
    // Setters
    void setName(const std::string& name);
    void setId(int id);
    void setScore(int score);
};
