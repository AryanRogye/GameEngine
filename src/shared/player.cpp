#include "player.h"
#include "configs.h"

Player::Player() 
{
    // Initialize the player's position
    this->position = {
        static_cast<int>(WIDTH / 2), static_cast<int>(HEIGHT / 2)
    };
    // Initialize the player's Widtht and Height
    this->width = 200;
    this->height = 200;
    // Initialize the player's speed
    this->speed = 120;
    this->isWalking = false;
    this->spriteIndex = -1;
    this->facing = Facing::FORWARD;
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
void Player::setSpeed(float speed) { this->speed = speed; }
void Player::setSpriteIndex(int spriteIndex) { this->spriteIndex = spriteIndex; }
void Player::setFacing(Facing facing) { this->facing = facing; }
void Player::setIsWalking(bool isWalking) { this->isWalking = isWalking; }
void Player::setName(const std::string& name)
{
    std::strncpy(this->name, name.c_str(), sizeof(this->name) - 1);
    this->name[sizeof(this->name) - 1] = '\0';
}

// Getter Functions
int         Player::getID() { return this->id; }
const char* Player::getName() { return this->name; }
int         Player::getScore() { return this->score; }
Position    Player::getPosition() { return this->position; }
float       Player::getX() { return this->position.x; }
float       Player::getY() { return this->position.y; }
int         Player::getWidth() { return this->width; }
int         Player::getHeight() { return this->height; }
float       Player::getSpeed() { return this->speed; }
int         Player::getSpriteIndex() { return this->spriteIndex; }
Facing      Player::getFacing() { return this->facing; }
bool        Player::getIsWalking() { return this->isWalking; }

SDL_Rect Player::getWorldHitbox() {
    return worldHitbox;
}
void Player::updateWorldHitbox( int offsetX, int offsetY ) {
    worldHitbox = {
        static_cast<int>(this->getX()) + offsetX + hitboxOffsetX,  // No more centering with sprite
        static_cast<int>(this->getY()) + offsetY + hitboxOffsetY,
        hitboxWidth,  // Fixed size for now, could be made configurable
        hitboxHeight
    };
}

size_t  Player::serialize(uint8_t *buffer)
{
    size_t offset = 0;
    
    // Serialize the player's id
    std::memcpy(buffer + offset, &this->id, sizeof(this->id));
    offset += sizeof(this->id);
    // Serialize the player's name
    std::memcpy(buffer + offset, this->name, sizeof(this->name));
    offset += sizeof(this->name);
    // Serialize the player's score
    std::memcpy(buffer + offset, &this->score, sizeof(this->score));
    offset += sizeof(this->score);
    // Serialize the player's position
    std::memcpy(buffer + offset, &this->position, sizeof(this->position));
    offset += sizeof(this->position);
    // Serialize the player's width
    std::memcpy(buffer + offset, &this->width, sizeof(this->width));
    offset += sizeof(this->width);
    // Serialize the player's height
    std::memcpy(buffer + offset, &this->height, sizeof(this->height));
    offset += sizeof(this->height);
    // Serialize the player's speed
    std::memcpy(buffer + offset, &this->speed, sizeof(this->speed));
    offset += sizeof(this->speed);

    return offset;
}
void Player::deserialize(const uint8_t *buffer, size_t *offset)
{
    // Deserialize the player's id
    std::memcpy(&this->id, buffer + *offset, sizeof(this->id));
    *offset += sizeof(this->id);
    // Deserialize the player's name
    std::memcpy(this->name, buffer + *offset, sizeof(this->name));
    *offset += sizeof(this->name);
    // Deserialize the player's score
    std::memcpy(&this->score, buffer + *offset, sizeof(this->score));
    *offset += sizeof(this->score);
    // Deserialize the player's position
    std::memcpy(&this->position, buffer + *offset, sizeof(this->position));
    *offset += sizeof(this->position);
    // Deserialize the player's width
    std::memcpy(&this->width, buffer + *offset, sizeof(this->width));
    *offset += sizeof(this->width);
    // Deserialize the player's height
    std::memcpy(&this->height, buffer + *offset, sizeof(this->height));
    *offset += sizeof(this->height);
    // Deserialize the player's speed
    std::memcpy(&this->speed, buffer + *offset, sizeof(this->speed));
    *offset += sizeof(this->speed);
}
