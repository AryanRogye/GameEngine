#include "player.h"

Player::Player(int id, const char* name) 
{
    // Initialize the player's position
    this->position = {20, 20};
    // Initialize the player's id
    this->id = id;
    // Initialize the player's name
    this->setName(name);
    // Initialize the player's Widtht and Height
    this->width = 20;
    this->height = 20;
}

// Setter Functions
void Player::setScore(int score) { this->score = score; }
void Player::setPosition(float x, float y) { this->position = {x, y}; }
void Player::setPosition(Position position) { this->position = position; }
void Player::setX(float x) { this->position.x = x; }
void Player::setY(float y) { this->position.y = y; }
void Player::setID(int id) { this->id = id; }
void Player::setWidth(int width) { this->width = width; }
void Player::setHeight(int height) { this->height = height; }
void Player::setName(const std::string& name) 
{
    std::strncpy(this->name, name.c_str(), sizeof(this->name) - 1);
    this->name[sizeof(this->name) - 1] = '\0';
}

// Getter Functions
int         Player::getId() { return this->id; }
const char* Player::getName() { return this->name; }
int         Player::getScore() { return this->score; }
Position    Player::getPosition() { return this->position; }
float       Player::getX() { return this->position.x; }
float       Player::getY() { return this->position.y; }
int         Player::getWidth() { return this->width; }
int         Player::getHeight() { return this->height; }



