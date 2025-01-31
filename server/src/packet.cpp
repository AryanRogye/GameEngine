#include "packet.h"

PacketType Serializable::get_packet_type(const uint8_t *buffer, size_t *offset) {
    // The First Byte of the buffer is the packet type
    PacketType type;
    std::memcpy(&type, buffer, sizeof(type));
    *offset += sizeof(type);
    return type;
}


PlayerJoined::PlayerJoined(const std::string& name)
{
    this->id = -1;
    std::memset(this->name, 0, sizeof(this->name));
    std::strncpy(this->name, name.c_str(), sizeof(this->name) - 1);
}

PlayerJoined::PlayerJoined(int id, const std::string& name) : id(id)
{
    std::memset(this->name, 0, sizeof(this->name));
    std::strncpy(this->name, name.c_str(), sizeof(this->name) - 1);
}

void PlayerJoined::setID(int id) { this->id = id; }
int PlayerJoined::getID() const { return this->id; }
void PlayerJoined::setName(const std::string& name) { std::strncpy(this->name, name.c_str(), sizeof(this->name) - 1); }
std::string PlayerJoined::getName() const { return std::string(this->name); }

size_t PlayerJoined::serialize(uint8_t* buffer)
{
    size_t offset = 0;
    // Serialize The Enum cooresponding to the packet type
    PacketType type = PacketType::PLAYER_JOINED;
    std::memcpy(buffer + offset, &type, sizeof(type));
    offset += sizeof(type);
    // First Serialize the id
    std::memcpy(buffer + offset, &this->id, sizeof(this->id));
    offset += sizeof(this->id);

    // Serialize the name
    std::memcpy(buffer + offset, this->name, sizeof(this->name));
    offset += sizeof(this->name);

    return offset;
}

void PlayerJoined::deserialize(const uint8_t *buffer, size_t *offset)
{
    // Deserialize id
    std::memcpy(&this->id, buffer + *offset, sizeof(this->id));
    *offset += sizeof(this->id);

    // Deserialize name content
    std::memcpy(this->name, buffer + *offset, sizeof(this->name));
    *offset += sizeof(this->name);
}

PlayerMoved::PlayerMoved(int id, float x, float y) 
{
    this->id = id;
    this->x = x;
    this->y = y;
}

void PlayerMoved::setID(int id) { this->id = id; }
int PlayerMoved::getID() const { return this->id; }
void PlayerMoved::setX(float x) { this->x = x; }
float PlayerMoved::getX() const { return this->x; }
void PlayerMoved::setY(float y) { this->y = y; }
float PlayerMoved::getY() const { return this->y; }

size_t PlayerMoved::serialize(uint8_t *buffer)
{
    size_t offset = 0;
    // Serialize The Enum cooresponding to the packet type
    PacketType type = PacketType::PLAYER_MOVED;
    std::memcpy(buffer + offset, &type, sizeof(type));
    offset += sizeof(type);
    // Serialize id
    std::memcpy(buffer + offset, &this->id, sizeof(this->id));
    offset += sizeof(this->id);

    // Serialize x
    std::memcpy(buffer + offset, &this->x, sizeof(this->x));
    offset += sizeof(this->x);

    // Serialize y
    std::memcpy(buffer + offset, &this->y, sizeof(this->y));
    offset += sizeof(this->y);

    return offset;
}

void PlayerMoved::deserialize(const uint8_t *buffer, size_t *offset)
{
    // Deserialize id
    std::memcpy(&this->id, buffer + *offset, sizeof(this->id));
    *offset += sizeof(this->id);

    // Deserialize x
    std::memcpy(&this->x, buffer + *offset, sizeof(this->x));
    *offset += sizeof(this->x);

    // Deserialize y
    std::memcpy(&this->y, buffer + *offset, sizeof(this->y));
    *offset += sizeof(this->y);
}

AssignPlayerID::AssignPlayerID(int id) 
{
    this->id = id;
}

int AssignPlayerID::getID() const { return this->id; }

size_t AssignPlayerID::serialize(uint8_t *buffer)
{
    size_t offset = 0;
    // Serialize The Enum cooresponding to the packet type
    PacketType type = PacketType::ASSIGN_PLAYER_ID;
    std::memcpy(buffer + offset, &type, sizeof(type));
    offset += sizeof(type);

    // Serialize id
    std::memcpy(buffer + offset, &this->id, sizeof(this->id));
    offset += sizeof(this->id);

    return offset;
}

void AssignPlayerID::deserialize(const uint8_t *buffer, size_t *offset)
{
    // Deserialize id
    std::memcpy(&this->id, buffer + *offset, sizeof(this->id));
    *offset += sizeof(this->id);
}

