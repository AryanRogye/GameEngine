#include "player.h"

Player::Player() {}
Player::Player(char* name, int id, int score) {}
Player::Player(char* name, int id) {}
Player::Player(char* name) {}

char* Player::getName() {
    return this->name;
}
int   Player::getId() {
    return this->id;
}
int   Player::getScore() {
    return this->score;
}

void  Player::setName(const std::string& name) {
    std::strncpy(this->name, name.c_str(), sizeof(this->name) - 1);
    this->name[sizeof(this->name) - 1] = '\0';
}
void  Player::setId(int id) {
    this->id = id;
}
void  Player::setScore(int score) {
    this->score = score;
}
