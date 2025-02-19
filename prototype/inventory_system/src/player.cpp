#include "player.h"

Player::Player()
{
    this->setX(0);
    this->setY(0);
    this->setHealth(100);
    this->setDamage(10);
    this->setLevel(1);
    this->setExperience(0);
    this->setMaxHealth(100);
    this->setRenderer(NULL);
    this->setSpeed(10);
}
Player::Player(int x, int y)
{
    this->setX(x);
    this->setY(y);
    this->setHealth(100);
    this->setDamage(10);
    this->setLevel(1);
    this->setExperience(0);
    this->setMaxHealth(100);
    this->setRenderer(NULL);
    this->setSpeed(10);
}

// Getters
int Player::getX() { return this->x; }
int Player::getY() { return this->y; }
int Player::getHealth() { return this->health; }
int Player::getDamage() { return this->damage; }
int Player::getLevel() { return this->level; }
int Player::getExperience() { return this->experience; }
int Player::getMaxHealth() { return this->maxHealth; }
SDL_Renderer* Player::getRenderer() { return this->renderer; }
int Player::getSpeed() { return this->speed; }
// Setters
void Player::setX(int x) { this->x = x; }
void Player::setY(int y) { this->y = y; }
void Player::setHealth(int health) { this->health = health; }
void Player::setDamage(int damage) { this->damage = damage; }
void Player::setLevel(int level) { this->level = level; }
void Player::setExperience(int experience) { this->experience = experience; }
void Player::setMaxHealth(int maxHealth) { this->maxHealth = maxHealth; }
void Player::setRenderer(SDL_Renderer* renderer) { this->renderer = renderer; }
void Player::setSpeed(int speed) { this->speed = speed; }
// Methods

void Player::loadPlayer() 
{ 
}
void Player::drawPlayer()
{
    if (!this->getRenderer())
    {
        std::cout << "No renderer set for player" << std::endl;
        return;
    }
    SDL_SetRenderDrawColor(this->getRenderer(), 0, 0, 255, 255);
    SDL_Rect playerRect = {this->getX(), this->getY(), 50, 50};
    SDL_RenderFillRect(this->getRenderer(), &playerRect);
}
void Player::handleInput(SDL_Event &event)
{
    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_w:
                this->setY(this->getY() - this->getSpeed());
                break;
            case SDLK_s:
                this->setY(this->getY() + this->getSpeed());
                break;
            case SDLK_a:
                this->setX(this->getX() - this->getSpeed());
                break;
            case SDLK_d:
                this->setX(this->getX() + this->getSpeed());
                break;
        }
    }
}
