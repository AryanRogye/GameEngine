#include "packet.h"

/**
 * Figure Out What the Packet Type is
 * @param buffer: The buffer that contains the packet
 * @param offset: The offset in the buffer where the packet type is located
 *  This gets updated to the next offset so that you can continue reading the rest of the packet
 **/
PacketType Serializable::get_packet_type(const uint8_t *buffer, size_t *offset) 
{
    // The First Byte of the buffer is the packet type
    PacketType type;
    std::memcpy(&type, buffer, sizeof(type));
    *offset += sizeof(type);
    return type;
}

// ############################################################
// ############# Player Joined Implementation #################
// ############################################################

/** Init A Player Joined Object with a name **/
PlayerJoined::PlayerJoined(const std::string& name)
{
    this->id = -1;
    std::memset(this->name, 0, sizeof(this->name));
    std::strncpy(this->name, name.c_str(), sizeof(this->name) - 1);
}

/** Init A Player Joined Object with an id and name **/
PlayerJoined::PlayerJoined(int id, const std::string& name) : id(id)
{
    std::memset(this->name, 0, sizeof(this->name));
    std::strncpy(this->name, name.c_str(), sizeof(this->name) - 1);
}

// Setters for PlayerJoined
void PlayerJoined::setID(int id) { this->id = id; }
void PlayerJoined::setName(const std::string& name) { std::strncpy(this->name, name.c_str(), sizeof(this->name) - 1); }
// Getters for PlayerJoined
int PlayerJoined::getID() const { return this->id; }
std::string PlayerJoined::getName() const { return std::string(this->name); }

/** Serialize the PlayerJoined Packet **/
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

/** Deserialize the PlayerJoined Packet **/
void PlayerJoined::deserialize(const uint8_t *buffer, size_t *offset)
{
    // Deserialize id
    std::memcpy(&this->id, buffer + *offset, sizeof(this->id));
    *offset += sizeof(this->id);

    // Deserialize name content
    std::memcpy(this->name, buffer + *offset, sizeof(this->name));
    *offset += sizeof(this->name);
}

// ############################################################
// ############### PlayerMoved Implementation #################
// ############################################################

/** Init The Player Moved with an id and x,y positions **/
PlayerMoved::PlayerMoved(int id, float x, float y) 
{
    this->id = id;
    this->x = x;
    this->y = y;
}

// Setters for PlayerMoved
void PlayerMoved::setID(int id) { this->id = id; }
void PlayerMoved::setX(float x) { this->x = x; }
void PlayerMoved::setY(float y) { this->y = y; }
// Getters for PlayerMoved
int PlayerMoved::getID() const { return this->id; }
float PlayerMoved::getX() const { return this->x; }
float PlayerMoved::getY() const { return this->y; }

/** Serialize the PlayerMoved Packet **/
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

/** Deserialize the PlayerMoved Packet **/
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

// ############################################################
// ############# AssignID Implementation ######################
// ############################################################

/** Init The AssignPlayerID with an id **/
AssignPlayerID::AssignPlayerID(int id) 
{
    this->id = id;
}
// Getters for AssignPlayerID
int AssignPlayerID::getID() const { return this->id; }

/** Serialize the AssignPlayerID Packet **/
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

/** Deserialize the AssignPlayerID Packet **/
void AssignPlayerID::deserialize(const uint8_t *buffer, size_t *offset)
{
    // Deserialize id
    std::memcpy(&this->id, buffer + *offset, sizeof(this->id));
    *offset += sizeof(this->id);
}


// ############################################################
// ##### SendExistingClientsToNewPlayer Implementation ########
// ############################################################

SendExistingClientsToNewPlayer::SendExistingClientsToNewPlayer(std::vector<Player>& players)
{
    this->players = players;
}

std::vector<Player>& SendExistingClientsToNewPlayer::getPlayers()
{
    return this->players;
}

size_t SendExistingClientsToNewPlayer::serialize(uint8_t *buffer)
{
    size_t offset = 0;

    // Serialize The Enum corresponding to the packet type
    PacketType type = PacketType::SEND_EXISTING_CLIENTS_NEW_PLAYER;
    std::memcpy(buffer + offset, &type, sizeof(type));
    offset += sizeof(type);

    // Serialize the number of players
    int num_players = this->players.size();
    std::memcpy(buffer + offset, &num_players, sizeof(num_players));
    offset += sizeof(num_players);

    // Serialize each Player object
    for (Player& player : this->players)
    {
        size_t playerSize = player.serialize(buffer + offset);
        offset += playerSize;
    }
    return offset;
}

void SendExistingClientsToNewPlayer::deserialize(const uint8_t *buffer, size_t *offset)
{
    // Deserialize player_ids
    int num_players;
    std::memcpy(&num_players, buffer + *offset, sizeof(num_players));
    *offset += sizeof(num_players);

    std::cout << "Got The Number of Players: " << num_players << std::endl;

    // Deserialize each player
    for (int i = 0; i < num_players; i++)
    {
        Player player;
        player.deserialize(buffer, offset);
        this->players.push_back(player);
    }
}

// ############################################################
// ###### SendToExisitingClientsNewPlayer Implementation ######
// ############################################################

SendToExisitingClientsNewPlayer::SendToExisitingClientsNewPlayer(int id)
{
    this->id = id;
}

size_t SendToExisitingClientsNewPlayer::serialize(uint8_t *buffer)
{
    // Serialize The Enum cooresponding to the packet type
    PacketType type = PacketType::SEND_NEW_PLAYER_EXISTING_CLIENTS;
    std::memcpy(buffer, &type, sizeof(type));
    size_t offset = sizeof(type);

    // Serialize id
    std::memcpy(buffer + offset, &this->id, sizeof(this->id));
    offset += sizeof(this->id);
    
    return offset;
}

void SendToExisitingClientsNewPlayer::deserialize(const uint8_t *buffer, size_t *offset)
{
    // Deserialize id
    std::memcpy(&this->id, buffer + *offset, sizeof(this->id));
    *offset += sizeof(this->id);
}

SendPlayerSpriteIndex::SendPlayerSpriteIndex(int id, int spriteIndex)
{
    this->id = id;
    this->spriteIndex = spriteIndex;
}
size_t SendPlayerSpriteIndex::serialize(uint8_t *buffer)
{
    PacketType type = PacketType::SEND_PLAYER_SPRITE_INDEX;
    std::memcpy(buffer, &type, sizeof(type));
    size_t offset = sizeof(type);

    // Serialize id
    std::memcpy(buffer + offset, &this->id, sizeof(this->id));
    offset += sizeof(this->id);

    // Serialize spriteIndex
    std::memcpy(buffer + offset, &this->spriteIndex, sizeof(this->spriteIndex));
    offset += sizeof(this->spriteIndex);

    return offset;
}

void SendPlayerSpriteIndex::deserialize(const uint8_t *buffer, size_t *offset)
{
    // Deserialize id
    std::memcpy(&this->id, buffer + *offset, sizeof(this->id));
    *offset += sizeof(this->id);

    // Deserialize spriteIndex
    std::memcpy(&this->spriteIndex, buffer + *offset, sizeof(this->spriteIndex));
    *offset += sizeof(this->spriteIndex);
}

int SendPlayerSpriteIndex::getID() const
{
    return this->id;
}

int SendPlayerSpriteIndex::getSpriteIndex() const
{
    return this->spriteIndex;
}



std::vector<std::vector<int>>& SendMapData::getMapData()
{
    return this->mapData;
}
size_t SendMapData::getSeralizedSize()
{
    size_t size = sizeof(PacketType) + sizeof(int); // for number of rows
    for (const auto &row : this->mapData)
    {
        size += sizeof(int); // for the row size
        size += row.size() * sizeof(int); // for the row data
    }
return size;
}
size_t SendMapData::serialize(uint8_t *buffer)
{
    PacketType type = PacketType::SEND_MAP_DATA;
    std::memcpy(buffer, &type, sizeof(type));
    size_t offset = sizeof(type);

    // Ensure mapData is not empty before accessing mapData[0]
    int num_rows = static_cast<int>(this->mapData.size());
    int num_cols = num_rows > 0 ? static_cast<int>(this->mapData[0].size()) : 0;

    std::memcpy(buffer + offset, &num_rows, sizeof(num_rows));
    offset += sizeof(num_rows);
    std::memcpy(buffer + offset, &num_cols, sizeof(num_cols));
    offset += sizeof(num_cols);

    // Serialize mapData safely
    for (const auto &row : this->mapData)
    {
        for (int value : row)
        {
            std::memcpy(buffer + offset, &value, sizeof(value));
            offset += sizeof(value);
        }
    }

    return offset;
}

SendMapData::SendMapData(std::vector<std::vector<int>> mapData)
{
    this->mapData = mapData;
}

void SendMapData::deserialize(const uint8_t *buffer, size_t *offset)
{
    // Clear the mapData
    this->mapData.clear();


    // Deserialize the number of rows
    int num_rows;
    std::memcpy(&num_rows, buffer + *offset, sizeof(num_rows));
    *offset += sizeof(num_rows);

    // Deserialize the number of columns
    int num_cols;
    std::memcpy(&num_cols, buffer + *offset, sizeof(num_cols));
    *offset += sizeof(num_cols);

    // Resize mapData beforehand for efficiency
    this->mapData.resize(num_rows, std::vector<int>(num_cols));

    // Deserialize the map mapData
    for (int i = 0; i < num_rows; i++)
    {
        std::vector<int> row;
        for (int j = 0; j < num_cols; j++)
        {
            int data;
            std::memcpy(&data, buffer + *offset, sizeof(data));
            *offset += sizeof(data);
            row.push_back(data);
        }
        this->mapData.push_back(row);
    }
}
