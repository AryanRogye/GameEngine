#include "player.h"

Player::Player()
{
    // We wanna load from a file
    this->loadPlayer();
    // Give a pointer to the debug gui to display its values
    DebugGUI::SetPlayer(this);
}

// Getters
float Player::getX() { return position.x; }
float Player::getY() { return position.y; }
int Player::getHealth() { return health; }
int Player::getDamage() { return damage; }
int Player::getLevel() { return level; }
int Player::getExperience() { return experience; }
int Player::getMaxHealth() { return maxHealth; }
SDL_Renderer* Player::getRenderer() { return renderer; }
float Player::getAcceleration() { return acceleration; }
float Player::getMaxSpeed() { return maxSpeed; }
float Player::getFriction() { return friction; }
float Player::getVelocityX() { return velocity.x; }
float Player::getVelocityY() { return velocity.y; }

// Setters
void Player::setX(float value) { position.x = value; }
void Player::setY(float value) { position.y = value; }
void Player::setHealth(int value) { health = value; }
void Player::setDamage(int value) { damage = value; }
void Player::setLevel(int value) { level = value; }
void Player::setExperience(int value) { experience = value; }
void Player::setMaxHealth(int value) { maxHealth = value; }
void Player::setRenderer(SDL_Renderer* newRenderer) { renderer = newRenderer; }
void Player::setAcceleration(float value) { acceleration = value; }
void Player::setMaxSpeed(float value) { maxSpeed = value; }
void Player::setFriction(float value) { friction = value; }
void Player::setVelocityX(float value) { velocity.x = value; }
void Player::setVelocityY(float value) { velocity.y = value; }
// Methods

void Player::loadPlayer() 
{ 
    fetchPlayerConfigs(this);
    DebugGUI::addDebugLog("Player File Loaded");
}
void Player::drawPlayer(float dt, float scale)
{
    if (!this->getRenderer())
    {
        std::cout << "No renderer set for player" << std::endl;
        return;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_FRect playerRect = {
        (int)position.x * scale,
        (int)position.y * scale,
        50 * scale,
        50 * scale
    };
    SDL_RenderFillRectF(renderer, &playerRect);
}
void Player::handleInput(SDL_Event &event, float dt)
{
    // Track key states
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        bool pressed = (event.type == SDL_KEYDOWN);
        switch (event.key.keysym.sym) {
            case SDLK_w: keysPressed[0] = pressed; break;
            case SDLK_s: keysPressed[1] = pressed; break;
            case SDLK_a: keysPressed[2] = pressed; break;
            case SDLK_d: keysPressed[3] = pressed; break;
        }
    }
}

void Player::update(float dt)
{
    Vec2 direction(0.0f, 0.0f);
    if (keysPressed[0]) direction.y -= 1.0f; // W
    if (keysPressed[1]) direction.y += 1.0f; // S
    if (keysPressed[2]) direction.x -= 1.0f; // A
    if (keysPressed[3]) direction.x += 1.0f; // D

    if (direction.x != 0.0f && direction.y != 0.0f) {
        direction = direction.normalize();
    }

    velocity.x += direction.x * acceleration * dt;
    velocity.y += direction.y * acceleration * dt;

    float currentSpeed = velocity.length();
    if (currentSpeed > maxSpeed) {
        velocity = velocity.normalize() * maxSpeed;
    }

    if (direction.x == 0.0f) {
        velocity.x *= pow(friction, dt * 60.0f);
    }
    if (direction.y == 0.0f) {
        velocity.y *= pow(friction, dt * 60.0f);
    }

    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    if (velocity.length() > 1.0f) {
        state = PlayerState::WALKING;
    } else {
        state = PlayerState::IDLE;
    }
}
