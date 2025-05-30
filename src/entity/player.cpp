#include "entity/player.h"
#include "Vec2.h"
#include "TSDL.h"
#include "comfy_lib.h"

Player::Player() :
// Setting Player Collision Here
collision(new Collision(this)),
camera(new Camera(this)),
sprite(new Sprites(this))
{
    // We wanna load from a file
    this->loadPlayer();
    // Give a pointer to the debug gui to display its values
    DebugGUI::SetPlayer(this);
    // lol be tiny at first
    this->width = 0;
    this->height = 0;
}

// Getters
float Player::getX() { return position.x; }
float Player::getY() { return position.y; }
float Player::getWidth() { return width; }
float Player::getHeight() { return height; }
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
Collision *Player::getCollision() { return collision; }
TSDL_TileMap *Player::getTileMap() { return tileMap; }
Player::PlayerState Player::getState() { return this->state; }
float Player::getPlayerScale() { return playerScale; }
Camera *Player::getCamera() { return camera; }
Sprites *Player::getSprite() { return sprite; }

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
void Player::setCollision(Collision *value) { collision = value; }
void Player::setTileMap(TSDL_TileMap *value) 
{ 
    tileMap = value; 
    this->width = tileMap->tileWidth;
    this->height = tileMap->tileHeight;
}
void Player::setState(Player::PlayerState value) { state = value; }
void Player::setPlayerScale(float value) { playerScale = value; }
void Player::setCamera(Camera *value) { camera = value; }
void Player::setSprite(Sprites *value) { sprite = value; }




// Methods

void Player::loadPlayer() 
{ 
    fetchPlayerConfigs(this);
    DebugGUI::addDebugLog("Player File Loaded", ErrorCode::SUCCESS);
}


// sprite is now ready so we can start drawing that instead of a rectangle
void Player::draw(float dt, float scale)
{
    if (this->playerScale != scale)
        this->playerScale = scale;

    if (!this->getRenderer())
    {
        std::cout << "No renderer set for player" << std::endl;
        return;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_FRect playerRect = {
        (position.x - this->camera->getX()) * scale,  // Offset by camera X
        (position.y - this->camera->getY()) * scale,  // Offset by camera Y
        this->width * scale,
        this->height * scale
    };
    SDL_RenderFillRectF(renderer, &playerRect);


    // Draw The Collision Box (This is a debug feature)
    // In Production You Wouldnt Want This
    this->collision->drawPlayerCollision();
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
    // Create a direction vector based on the keys pressed
    Vec2 direction(0.0f, 0.0f);
    bool isMoving = false;
    // W, S, A, D
    if (keysPressed[0]) 
    {
        direction.y -= 1.0f; // W
        isMoving = true;
    }
    if (keysPressed[1]) 
    {
        direction.y += 1.0f; // S
        isMoving = true;
    }
    if (keysPressed[2]) 
    {
        direction.x -= 1.0f; // A
        isMoving = true;
    }
    if (keysPressed[3]) 
    {
        direction.x += 1.0f; // D
        isMoving = true;
    }

    // Normalize the direction vector if moving diagonally
    if (direction.x != 0.0f && direction.y != 0.0f) 
    {
        direction = direction.normalize();
    }

    // Update the velocity based on the direction
    velocity.x += direction.x * acceleration * dt;
    velocity.y += direction.y * acceleration * dt;

    // Clamp the velocity to the max speed
    float currentSpeed = velocity.length();
    if (currentSpeed > maxSpeed) 
    {
        velocity = velocity.normalize() * maxSpeed;
    }

    // Apply friction
    if (direction.x == 0.0f) 
    {
        velocity.x *= pow(friction, dt * 60.0f);
    }
    
    if (direction.y == 0.0f) 
    {
        velocity.y *= pow(friction, dt * 60.0f);
    }

    // Used For Collision
    Vec2 previousPosition = position;

    // TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO This funciton now works perfectly but its not optimized to be the best it can be
    // TODO so when i dont feel lazy and tired i'll do it then
    // TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    bool isColliding = false;

    // Update the x pos and check if its colliding
    position.x += velocity.x * dt;
    if (this->collision->collidesWithMapLayer(this->getTileMap(), this->getPlayerScale()))
    {
        // if it does collide then we set the position back to the previous position
        position.x = previousPosition.x;
        this->setState(PlayerState::COLLIDING);
        isColliding = true;
        velocity.x = 0; // Stop the player from moving
    }

    // Update the y pos and check if its colliding
    position.y += velocity.y * dt;
    if (this->collision->collidesWithMapLayer(this->getTileMap(), this->getPlayerScale()))
    {
        // if it does collide then we set the position boxack to the previous position
        position.y = previousPosition.y;
        this->setState(PlayerState::COLLIDING);
        isColliding = true;
        velocity.y = 0; // Stop the player from moving
    }

    if (this->tileMap)
    {
        // Ensure the camera doesn't go beyond the map boundaries
        float mapWidth =
            this->getTileMap()->width * this->getTileMap()->tileWidth;
        float mapHeight =
            this->getTileMap()->height * this->getTileMap()->tileHeight;

        float camMinX = 0;
        float camMinY = 0;
        float camMaxX = mapWidth - this->camera->getWidth();
        float camMaxY = mapHeight - this->camera->getHeight();

        // Set the new camera position and clamp it
        float newCamX = this->position.x - (this->camera->getWidth() / 2);
        float newCamY = this->position.y - (this->camera->getHeight() / 2);

        this->camera->setX(std::max(camMinX, std::min(newCamX, camMaxX)));
        this->camera->setY(std::max(camMinY, std::min(newCamY, camMaxY)));
    }

    // Update the collision box
    if (this->collision->collidesWithMapLayer(this->getTileMap(), playerScale)) 
    {
        this->setState(PlayerState::COLLIDING);
        isColliding = true;
    }
    if (
        (isColliding && isMoving) || 
        (this->collision->collidesWithMapLayer(this->getTileMap(), playerScale))
    )
    {
        this->setState(PlayerState::COLLIDING);
    }
    // If the player is moving
    else 
    {
        if (velocity.length() > 1.0f) 
        {
            state = PlayerState::WALKING;
        } 
        else 
        {
            state = PlayerState::IDLE;
        }
    }
}
