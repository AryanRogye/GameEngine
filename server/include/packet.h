#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <pthread/qos.h>

enum PacketType 
{
    PLAYER_JOINED,
    PLAYER_MOVED,
};

class Serializable
{
public:
    virtual size_t serialize(uint8_t *buffer) = 0;
    /** 
     * call this after the you have identified which packet type you are dealing with 
    **/
    virtual void deserialize(const uint8_t *buffer, size_t *offset) = 0;
    virtual ~Serializable() = default;

    /** 
     * Figure Out What the Packet Type is
     * @param buffer: The buffer that contains the packet
     * @param offset: The offset in the buffer where the packet type is located
     *  This gets updated to the next offset so that you can continue reading the rest of the packet
     **/
    static PacketType get_packet_type(const uint8_t *buffer, size_t *offset) {
        // The First Byte of the buffer is the packet type
        PacketType type;
        std::memcpy(&type, buffer, sizeof(type));
        *offset += sizeof(type);
        return type;
    }
};

class PlayerJoined : public Serializable
{
private:
    int id;
    char name[32];
public:
    void set_id(int id) { this->id = id; }
    PlayerJoined() = default;
    PlayerJoined(const std::string& name)
    {
        this->id = -1;
        std::memset(this->name, 0, sizeof(this->name));
        std::strncpy(this->name, name.c_str(), sizeof(this->name) - 1);
    }
    PlayerJoined(int id = -1, const std::string& name = "") : id(id) 
    {
        std::memset(this->name, 0, sizeof(this->name));
        std::strncpy(this->name, name.c_str(), sizeof(this->name) - 1);
    }

    size_t serialize(uint8_t *buffer) override
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
    /**
     * Same as deserialize without offset just added this in case
     * offset was already calculated before and we need to add onto it
     **/
    void deserialize(const uint8_t *buffer, size_t *offset) override
    {
        // Deserialize id
        std::memcpy(&this->id, buffer + *offset, sizeof(this->id));
        *offset += sizeof(this->id);

        // Deserialize name content
        std::memcpy(this->name, buffer + *offset, sizeof(this->name));
        *offset += sizeof(this->name);

        // ✅ Debugging: Print Raw Buffer Content
        /*std::cout << "Serialized Buffer (HEX): ";*/
        /*for (size_t i = 0; i < *offset; i++) {*/
        /*    std::cout << std::hex << static_cast<int>(buffer[i]) << " ";*/
        /*}*/
        /*std::cout << std::dec << std::endl;*/
    }

    int get_id() const { return this->id; }
    std::string get_name() const { return std::string(this->name); }
};

class PlayerMoved : Serializable
{
private:
    int id;
    float x;
    float y;
public:
    PlayerMoved(int id = 0, float x = 0.0f, float y = 0.0f) : id(id), x(x), y(y) {}
    size_t serialize(uint8_t *buffer) override
    {
        size_t offset = 0;
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
    void deserialize(const uint8_t *buffer, size_t *offset) override
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
};

